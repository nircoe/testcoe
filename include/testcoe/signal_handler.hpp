#pragma once

#include <csignal>
#include <iostream>
#include <string>

namespace testcoe
{
    extern std::streambuf *g_originalCoutBuf;
    extern std::streambuf *g_originalCerrBuf;

#ifdef _WIN32
    void signalHandler(int signal);
#else
    void signalHandler(int signal, siginfo_t *info, void *context);
#endif
    void installSignalHandlers();
    void setupStackTraceEnhancements();
} // namespace testcoe