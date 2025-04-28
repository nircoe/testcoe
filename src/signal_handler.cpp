#include <testcoe/signal_handler.hpp>
#include <cstdlib>
#define BACKWARD_HAS_BFD 0
#include <backward.hpp>

namespace testcoe
{
    std::streambuf *g_originalCoutBuf = nullptr;
    std::streambuf *g_originalCerrBuf = nullptr;

    void signalHandler(int signal)
    {
        if(g_originalCoutBuf)
            std::cout.rdbuf(g_originalCoutBuf);
        if(g_originalCerrBuf)
            std::cerr.rdbuf(g_originalCerrBuf);

        std::cerr << std::endl << std::endl << "====== TEST TERMINATED BY SIGNAL ======" << std::endl;
        std::cerr << "Test crashed with signal " << signal;

        if(signal == SIGSEGV)
            std::cerr << " (SIGSEGV: Segmentation fault - likely memory access violation)";
        else if(signal == SIGABRT)
            std::cerr << " (SIGABRT: Abort - likely assertion failure or std::abort call)";
        else if(signal == SIGFPE)
            std::cerr << " (SIGFPE: Floating point exception)";
        else if(signal == SIGILL)
            std::cerr << " (SIGILL: Illegal instruction)";
        else if(signal == SIGTERM)
            std::cerr << " (SIGTERM: Termination request)";
        else if(signal == SIGINT)
            std::cerr << " (SIGINT: Interrupt)";

        std::cerr << std::endl;
        std::cerr << std::endl << "Stack trace:" << std::endl;
        
        backward::StackTrace stacktrace;
        stacktrace.load_here();

        backward::Printer printer;
        printer.object = true;
        printer.color_mode = backward::ColorMode::always;
        printer.address = true;

        printer.print(stacktrace, std::cerr);

        std::cerr << std::endl
                  << "===== END OF CRASH REPORT =====" << std::endl
                  << std::endl;

        exit(128 + signal);
    }

    void installSignalHandlers()
    {
        signal(SIGSEGV, signalHandler);
        signal(SIGABRT, signalHandler);
        signal(SIGFPE, signalHandler);
        signal(SIGILL, signalHandler);
        signal(SIGTERM, signalHandler);
        signal(SIGINT, signalHandler);
    }
} // namespace testcoe