#pragma once

#include <csignal>
#include <iostream>
#include <string>

namespace testcoe
{
    extern std::streambuf *g_originalCoutBuf;
    extern std::streambuf *g_originalCerrBuf;

    void signalHandler(int signal);
    void installSignalHandlers();
} // namespace testcoe