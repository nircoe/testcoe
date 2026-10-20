#pragma once

#include <csignal>
#include <iostream>
#include <string>

namespace testcoe
{
    extern std::streambuf *g_original_cout_buf;
    extern std::streambuf *g_original_cerr_buf;

#ifdef _WIN32
    void signal_handler(int signal);
#else
    void signal_handler(int signal, siginfo_t *info, void *context);
#endif
    void install_signal_handlers();
    void setup_stack_trace_enhancements();
} // namespace testcoe
