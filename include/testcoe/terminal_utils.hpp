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
        bool isAnsiEnabled();
    } // namespace terminal
} // namespace testcoe