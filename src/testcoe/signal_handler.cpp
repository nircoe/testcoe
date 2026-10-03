#include <testcoe/signal_handler.hpp>
#include <testcoe/stack_trace.hpp>
#include <cstdlib>
#include <iostream>
#include <gtest/gtest.h>

// Windows-specific includes for SEH and better stack traces
#ifdef _WIN32
#include <windows.h>
#endif

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

        internal::print_stack_trace(std::cerr);

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
#endif

    void signalHandler(int signal)
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

        internal::print_stack_trace(std::cerr);

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

        signal(SIGSEGV, signalHandler);
        signal(SIGABRT, signalHandler);
        signal(SIGFPE, signalHandler);
        signal(SIGILL, signalHandler);
        signal(SIGTERM, signalHandler);
        signal(SIGINT, signalHandler);

        setupStackTraceEnhancements();

        std::cout << "Signal handlers installed successfully." << std::endl;
    }
} // namespace testcoe