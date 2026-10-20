#include <testcoe/signal_handler.hpp>
#include <testcoe/stack_trace.hpp>
#include <gtest/gtest.h>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#ifdef _WIN32
    #include <windows.h>
#endif
#include <signal.h>

namespace testcoe
{
    std::streambuf *g_original_cout_buf = nullptr;
    std::streambuf *g_original_cerr_buf = nullptr;

#ifdef _WIN32
    LONG WINAPI windows_exception_handler(EXCEPTION_POINTERS *exception_pointers)
    {
        OutputDebugStringA("Windows exception handler called\n");

        if (g_original_cout_buf) std::cout.rdbuf(g_original_cout_buf);
        if (g_original_cerr_buf) std::cerr.rdbuf(g_original_cerr_buf);

        std::cout.flush();
        std::cerr.flush();

        std::cerr << std::endl << std::endl << "====== TEST TERMINATED BY EXCEPTION ======" << std::endl;

        if (!exception_pointers || !exception_pointers->ExceptionRecord)
        {
            std::cerr << "Invalid exception pointer!" << std::endl;
            std::cerr.flush();
            ExitProcess(1);
        }

        DWORD exception_code = exception_pointers->ExceptionRecord->ExceptionCode;

        std::cerr << "Windows Exception Code: 0x" << std::hex << exception_code << std::dec;

        switch (exception_code)
        {
            case EXCEPTION_ACCESS_VIOLATION:
                std::cerr << " (ACCESS_VIOLATION: Segmentation fault - memory access violation)";
                break;
            case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
                std::cerr << " (ARRAY_BOUNDS_EXCEEDED: Array index out of bounds)";
                break;
            case EXCEPTION_INT_DIVIDE_BY_ZERO:
                std::cerr << " (INT_DIVIDE_BY_ZERO: Integer division by zero)";
                break;
            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                std::cerr << " (FLT_DIVIDE_BY_ZERO: Floating point division by zero)";
                break;
            case EXCEPTION_STACK_OVERFLOW:
                std::cerr << " (STACK_OVERFLOW: Stack overflow)";
                break;
            case EXCEPTION_ILLEGAL_INSTRUCTION:
                std::cerr << " (ILLEGAL_INSTRUCTION: Illegal instruction)";
                break;
            default:
                std::cerr << " (Unknown Windows exception)";
                break;
        }

        std::cerr << std::endl;
        std::cerr.flush();

        const auto fault_pc = reinterpret_cast<std::uintptr_t>(exception_pointers->ExceptionRecord->ExceptionAddress);
        internal::print_stack_trace(std::cerr, fault_pc);

        std::cerr << std::endl << "===== END OF CRASH REPORT =====" << std::endl << std::endl;

        std::cerr.flush();
        std::cout.flush();

        // give the console time to show the output
        Sleep(500);

        ExitProcess(1);
    }
#else
    namespace internal
    {
        namespace
        {
            std::uintptr_t get_fault_pc([[maybe_unused]] void *context)
            {
    #if defined(__linux__) && defined(__x86_64__)
                return static_cast<std::uintptr_t>(static_cast<ucontext_t *>(context)->uc_mcontext.gregs[REG_RIP]);
    #elif defined(__linux__) && defined(__aarch64__)
                return static_cast<std::uintptr_t>(static_cast<ucontext_t *>(context)->uc_mcontext.pc);
    #elif defined(__APPLE__) && defined(__aarch64__)
                const auto &state = static_cast<ucontext_t *>(context)->uc_mcontext->__ss;
        #ifdef __darwin_arm_thread_state64_get_pc
                return static_cast<std::uintptr_t>(__darwin_arm_thread_state64_get_pc(state));
        #else
                return static_cast<std::uintptr_t>(state.__pc);
        #endif
    #elif defined(__APPLE__) && defined(__x86_64__)
                return static_cast<std::uintptr_t>(static_cast<ucontext_t *>(context)->uc_mcontext->__ss.__rip);
    #else
                return 0;
    #endif
            }
        } // namespace
    } // namespace internal
#endif

#ifdef _WIN32
    void signal_handler(int signal)
#else
    void signal_handler(int signal, siginfo_t *, void *context)
#endif
    {
        if (g_original_cout_buf) std::cout.rdbuf(g_original_cout_buf);
        if (g_original_cerr_buf) std::cerr.rdbuf(g_original_cerr_buf);

        std::cerr << std::endl << std::endl << "====== TEST TERMINATED BY SIGNAL ======" << std::endl;
        std::cerr << "Test crashed with signal " << signal;

        if (signal == SIGSEGV)
            std::cerr << " (SIGSEGV: Segmentation fault - likely memory access violation)";
        else if (signal == SIGABRT)
            std::cerr << " (SIGABRT: Abort - likely assertion failure or std::abort call)";
        else if (signal == SIGFPE)
            std::cerr << " (SIGFPE: Floating point exception)";
        else if (signal == SIGILL)
            std::cerr << " (SIGILL: Illegal instruction)";
        else if (signal == SIGTERM)
            std::cerr << " (SIGTERM: Termination request)";
        else if (signal == SIGINT)
            std::cerr << " (SIGINT: Interrupt)";

        std::cerr << std::endl;

#ifdef _WIN32
        std::uintptr_t fault_pc = 0;
    #ifdef _MSC_VER
        // only set while the CRT runs this handler for a hardware exception, null for raise()
        const auto *info = static_cast<EXCEPTION_POINTERS *>(_pxcptinfoptrs);
        if (info && info->ExceptionRecord)
            fault_pc = reinterpret_cast<std::uintptr_t>(info->ExceptionRecord->ExceptionAddress);
    #endif
        internal::print_stack_trace(std::cerr, fault_pc);
#else
        internal::print_stack_trace(std::cerr, internal::get_fault_pc(context));
#endif

        std::cerr << std::endl << "===== END OF CRASH REPORT =====" << std::endl << std::endl;

        exit(128 + signal);
    }

    void setup_stack_trace_enhancements()
    {
#ifdef _WIN32
        SetUnhandledExceptionFilter(windows_exception_handler);

        std::cout << "Windows stack trace enhancements enabled." << std::endl;
#endif
    }

    void install_signal_handlers()
    {
        std::cout << "Installing signal handlers..." << std::endl;

        testing::GTEST_FLAG(catch_exceptions) = false;
        std::cout << "Disabled Google Test exception catching for better crash reporting." << std::endl;

        internal::warm_up_stack_trace();

#ifdef _WIN32
        signal(SIGABRT, signal_handler);
        signal(SIGTERM, signal_handler);
        signal(SIGINT, signal_handler);

    #ifdef _MSC_VER
        // The CRT catches hardware faults before windows_exception_handler on MSVC, so these need
        // signal(). signal_handler reads the fault address from the CRT exception pointers. On
        // MinGW signal() would catch them too but without the address, so they are left out.
        signal(SIGSEGV, signal_handler);
        signal(SIGFPE, signal_handler);
        signal(SIGILL, signal_handler);
    #endif
#else
        struct sigaction action{};
        action.sa_sigaction = signal_handler;
        action.sa_flags = SA_SIGINFO;
        sigemptyset(&action.sa_mask);

        for (int sig : {SIGSEGV, SIGABRT, SIGFPE, SIGILL, SIGTERM, SIGINT})
            sigaction(sig, &action, nullptr);
#endif

        setup_stack_trace_enhancements();

        std::cout << "Signal handlers installed successfully." << std::endl;
    }
} // namespace testcoe
