#include <testcoe/terminal_utils.hpp>

namespace testcoe
{
    namespace terminal
    {
        /**
         * @brief Checks if the terminal supports ANSI escape sequences
         *
         * Determines whether the current terminal supports ANSI escape sequences
         * for colored output and cursor manipulation. On most Unix-like systems,
         * this will return true. On Windows, it depends on the Windows version and
         * terminal type.
         *
         * This function is used internally to determine whether to use color codes
         * or fall back to plain text output. It enables the library to work correctly
         * across different platforms and terminal types.
         *
         * @return true if ANSI escape sequences are supported, false otherwise
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
         *
         * This function clears the terminal screen in a platform-independent way.
         * It uses ANSI escape sequences on platforms that support them and
         * falls back to platform-specific APIs when necessary (e.g., on Windows
         * without ANSI support).
         */
        void clear()
        {
#ifdef _WIN32
            if(!isAnsiEnabled())
            {
                // Fallback to Windows Console API since ANSI sequences aren't supported
                HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
                COORD coordScreen = {0, 0};
                DWORD cCharsWritten;
                CONSOLE_SCREEN_BUFFER_INFO csbi;

                if(hConsole == INVALID_HANDLE_VALUE || !GetConsoleScreenBufferInfo(hConsole, &csbi))
                    return; // cannot clear screen, continue as it is

                DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
                FillConsoleOutputCharacter(hConsole, ' ', dwConSize, coordScreen, &cCharsWritten);
                SetConsoleCursorPosition(hConsole, coordScreen);
                return;
            }
#endif
            // Use ANSI escape sequences for Unix-like systems and Windows with ANSI support
            // \033[H moves cursor to top-left corner
            // \033[J clears the screen from cursor to end
            std::cout << "\033[H\033[J";
        }
    } // namespace terminal
} // namespace testcoe