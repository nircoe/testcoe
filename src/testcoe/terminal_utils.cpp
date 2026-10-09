#include <testcoe/terminal_utils.hpp>

#ifdef _WIN32
    #include <io.h>
#else
    #include <unistd.h>
#endif
#include <cstdio>

namespace testcoe
{
    namespace terminal
    {
        /**
         * @brief Checks if the terminal handles ANSI escape codes (on Windows this also turns them on)
         * @return true if ANSI escape codes can be used
         */
        bool isAnsiEnabled()
        {
#ifdef _WIN32
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            DWORD dwMode = 0;

            if (hOut == INVALID_HANDLE_VALUE || !GetConsoleMode(hOut, &dwMode))
                return false;

            return (bool)SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#else
            return true;
#endif
        }

        /**
         * @brief Clears the terminal screen
         */
        void clear()
        {
#ifdef _WIN32
            if(!isAnsiEnabled())
            {
                HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
                COORD coordScreen = {0, 0};
                DWORD cCharsWritten;
                CONSOLE_SCREEN_BUFFER_INFO csbi;

                if(hConsole == INVALID_HANDLE_VALUE || !GetConsoleScreenBufferInfo(hConsole, &csbi))
                    return; // can't clear, leave the screen as it is

                DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
                FillConsoleOutputCharacter(hConsole, ' ', dwConSize, coordScreen, &cCharsWritten);
                SetConsoleCursorPosition(hConsole, coordScreen);
                return;
            }
#endif
            // \033[H moves cursor to top-left corner
            // \033[J clears the screen from cursor to end
            std::cout << "\033[H\033[J";
        }

        // Distinct from isAnsiEnabled(): TTY-ness, not ANSI capability. A pipe is never
        // interactive regardless of color support.
        bool isInteractive()
        {
#ifdef _WIN32
            return _isatty(_fileno(stdout)) != 0;
#else
            return isatty(STDOUT_FILENO) != 0;
#endif
        }
    } // namespace terminal
} // namespace testcoe