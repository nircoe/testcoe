#include <testcoe/signal_handler.hpp>
#include <testcoe/stack_trace.hpp>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <gtest/gtest.h>

// Windows-specific includes for SEH and better stack traces
#ifdef _WIN32
#include <windows.h>
#endif
#include <signal.h>

namespace testcoe
{
    std::streambuf *g_originalCoutBuf = nullptr;
    std::streambuf *g_originalCerrBuf = nullptr;

#ifdef _WIN32
    // Windows structured exception handler for better crash detection
    LONG WINAPI windowsExceptionHandler(EXCEPTION_POINTERS *pExceptionPtrs)
    {
        // Immediate debug output to see if we even get here
        OutputDebugStringA("Windows exception handler called\n");

        // Restore streams first
        if (g_originalCoutBuf)
            std::cout.rdbuf(g_originalCoutBuf);
        if (g_originalCerrBuf)
            std::cerr.rdbuf(g_originalCerrBuf);

        // Flush any pending output first
        std::cout.flush();
        std::cerr.flush();

        std::cerr << std::endl
                  << std::endl
                  << "====== TEST TERMINATED BY EXCEPTION ======" << std::endl;

        // Validate exception pointer before using it
        if (!pExceptionPtrs || !pExceptionPtrs->ExceptionRecord)
        {
            std::cerr << "Invalid exception pointer!" << std::endl;
            std::cerr.flush();
            ExitProcess(1);
            return EXCEPTION_EXECUTE_HANDLER;
        }

        // Store exception code safely
        DWORD exceptionCode = pExceptionPtrs->ExceptionRecord->ExceptionCode;

        std::cerr << "Windows Exception Code: 0x" << std::hex << exceptionCode << std::dec;

        // Translate common Windows exceptions to readable messages
        switch (exceptionCode)
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

        const auto fault_pc = reinterpret_cast<std::uintptr_t>(pExceptionPtrs->ExceptionRecord->ExceptionAddress);
        internal::print_stack_trace(std::cerr, fault_pc);

        std::cerr << std::endl
                  << "===== END OF CRASH REPORT =====" << std::endl
                  << std::endl;

        // Ensure all output is flushed before terminating
        std::cerr.flush();
        std::cout.flush();

        // Give the console time to process the output
        Sleep(500); // Increased delay

        // Terminate the process
        ExitProcess(1);
        return EXCEPTION_EXECUTE_HANDLER;
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
    void signalHandler(int signal)
#else
    void signalHandler(int signal, siginfo_t *, void *context)
#endif
    {
        if (g_originalCoutBuf)
            std::cout.rdbuf(g_originalCoutBuf);
        if (g_originalCerrBuf)
            std::cerr.rdbuf(g_originalCerrBuf);

        std::cerr << std::endl
                  << std::endl
                  << "====== TEST TERMINATED BY SIGNAL ======" << std::endl;
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

        std::cerr << std::endl
                  << "===== END OF CRASH REPORT =====" << std::endl
                  << std::endl;

        exit(128 + signal);
    }

    void setupStackTraceEnhancements()
    {
#ifdef _WIN32
        // Install Windows structured exception handler
        SetUnhandledExceptionFilter(windowsExceptionHandler);

        std::cout << "Windows stack trace enhancements enabled." << std::endl;
#endif
    }

    void installSignalHandlers()
    {
        std::cout << "Installing signal handlers..." << std::endl;

        testing::GTEST_FLAG(catch_exceptions) = false;
        std::cout << "Disabled Google Test exception catching for better crash reporting." << std::endl;

        internal::warm_up_stack_trace();

#ifdef _WIN32
        signal(SIGABRT, signalHandler);
        signal(SIGTERM, signalHandler);
        signal(SIGINT, signalHandler);

    #ifdef _MSC_VER
        // The CRT catches hardware faults before windowsExceptionHandler on MSVC, so these need
        // signal(). signalHandler reads the fault address from the CRT exception pointers. On
        // MinGW signal() would catch them too but without the address, so they are left out.
        signal(SIGSEGV, signalHandler);
        signal(SIGFPE, signalHandler);
        signal(SIGILL, signalHandler);
    #endif
#else
        struct sigaction action{};
        action.sa_sigaction = signalHandler;
        action.sa_flags = SA_SIGINFO;
        sigemptyset(&action.sa_mask);

        for (int sig : {SIGSEGV, SIGABRT, SIGFPE, SIGILL, SIGTERM, SIGINT})
            sigaction(sig, &action, nullptr);
#endif

        setupStackTraceEnhancements();

        std::cout << "Signal handlers installed successfully." << std::endl;
    }
} // namespace testcoe