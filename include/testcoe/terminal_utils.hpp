#pragma once

#include <iostream>

#ifdef _WIN32
    #include <windows.h>
#endif

namespace testcoe
{
    namespace terminal
    {
        void clear();
        bool is_ansi_enabled();
        bool is_interactive();
    } // namespace terminal
} // namespace testcoe
