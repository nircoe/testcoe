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
        bool is_ansi_enabled()
        {
#ifdef _WIN32
            HANDLE output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
            DWORD mode = 0;

            if (output_handle == INVALID_HANDLE_VALUE || !GetConsoleMode(output_handle, &mode)) return false;

            return (bool)SetConsoleMode(output_handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
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
            if (!is_ansi_enabled())
            {
                HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
                COORD origin = {0, 0};
                DWORD chars_written;
                CONSOLE_SCREEN_BUFFER_INFO csbi;

                if (console == INVALID_HANDLE_VALUE || !GetConsoleScreenBufferInfo(console, &csbi))
                    return; // can't clear, leave the screen as it is

                DWORD console_size = csbi.dwSize.X * csbi.dwSize.Y;
                FillConsoleOutputCharacter(console, ' ', console_size, origin, &chars_written);
                SetConsoleCursorPosition(console, origin);
                return;
            }
#endif
            // \033[H moves cursor to top-left corner
            // \033[J clears the screen from cursor to end
            std::cout << "\033[H\033[J";
        }

        // Distinct from is_ansi_enabled(): TTY-ness, not ANSI capability. A pipe is never
        // interactive regardless of color support.
        bool is_interactive()
        {
#ifdef _WIN32
            return _isatty(_fileno(stdout)) != 0;
#else
            return isatty(STDOUT_FILENO) != 0;
#endif
        }
    } // namespace terminal
} // namespace testcoe
