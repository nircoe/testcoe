#include <testcoe/stack_trace.hpp>
#include <testcoe_config.hpp>

#include <cstddef>
#include <cstdint>
#include <ios>
#include <string>

#if TESTCOE_STACKTRACE_BACKEND_STD
    #include <algorithm>
    #include <cctype>
    #include <filesystem>
    #include <fstream>
    #include <iomanip>
    #include <stacktrace>
    #include <type_traits>
    #include <vector>
#elif TESTCOE_STACKTRACE_BACKEND_EXECINFO
    #include <execinfo.h>
#elif defined(_WIN32) && TESTCOE_STACKTRACE_BACKEND_NONE
    #include <windows.h>
#endif

#if !defined(_WIN32) && (TESTCOE_STACKTRACE_BACKEND_STD || TESTCOE_STACKTRACE_BACKEND_EXECINFO)
    #include <cxxabi.h>
    #include <dlfcn.h>
    #include <cstdlib>
#endif

namespace testcoe
{
    namespace internal
    {
        namespace
        {
            [[maybe_unused]] constexpr std::size_t MAX_FRAMES = 48;

            struct frame
            {
                std::uintptr_t address;
                std::string object;
                std::string function;
                std::string file;
                std::uint32_t line;
            };

            [[maybe_unused]] void print_frame(std::ostream &out, std::size_t index, const frame &f)
            {
                out << "#" << index << " 0x" << std::hex << f.address << std::dec;
                if (!f.function.empty())
                    out << " in " << f.function;
                if (!f.file.empty())
                {
                    out << " at " << f.file;
                    if (f.line != 0)
                        out << ":" << f.line;
                }
                if (!f.object.empty())
                    out << " [" << f.object << "]";
                out << "\n";
            }

#if !defined(_WIN32) && (TESTCOE_STACKTRACE_BACKEND_STD || TESTCOE_STACKTRACE_BACKEND_EXECINFO)
            void resolve_with_dladdr(frame &f)
            {
                Dl_info info{};
                if (!dladdr(reinterpret_cast<void *>(f.address), &info))
                    return;

                if (info.dli_fname)
                    f.object = info.dli_fname;
                if (!info.dli_sname)
                    return;

                int status = 0;
                char *demangled = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
                f.function = (status == 0 && demangled) ? demangled : info.dli_sname;
                std::free(demangled);
            }
#endif
        } // namespace

#if TESTCOE_STACKTRACE_BACKEND_STD

        void warm_up_stack_trace()
        {
            // first call to std::stacktrace::current() lazily reads debug info (dl_iterate_phdr etc),
            // none of that is async-signal-safe, so do it once here instead of inside the signal handler
            [[maybe_unused]] const std::string warm_up = std::to_string(std::stacktrace::current(0, 1));
        }

        template <typename Handle>
        std::uintptr_t to_address(Handle handle)
        {
            if constexpr (std::is_pointer_v<Handle>)
                return reinterpret_cast<std::uintptr_t>(handle);
            else
                return static_cast<std::uintptr_t>(handle);
        }

        std::string normalize_path(std::string path)
        {
            std::replace(path.begin(), path.end(), '\\', '/');
#ifdef _WIN32
            std::transform(path.begin(), path.end(), path.begin(),
                           [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
#endif
            return path;
        }

        bool is_own_source(const std::string &file)
        {
            const std::string own_dir =
                normalize_path(std::filesystem::path(__FILE__).parent_path().string()) + "/";
            return normalize_path(file).rfind(own_dir, 0) == 0;
        }

        // prints the lines around the crash line, returns false if the file couldn't be read
        bool print_source_snippet(std::ostream &out, const std::string &file, std::uint32_t line)
        {
            std::ifstream in(file);
            if (!in)
                return false;

            const std::uint32_t first = line > 2 ? line - 2 : 1;
            const std::uint32_t last = line + 2;

            std::vector<std::string> lines;
            std::string text;
            for (std::uint32_t number = 1; number <= last && std::getline(in, text); ++number)
                if (number >= first)
                    lines.push_back(text);

            if (lines.empty())
                return false;

            const int width = static_cast<int>(std::to_string(first + lines.size() - 1).size());
            for (std::size_t i = 0; i < lines.size(); ++i)
            {
                const std::uint32_t number = first + static_cast<std::uint32_t>(i);
                out << (number == line ? "  > " : "    ") << std::setw(width) << number << " | " << lines[i] << "\n";
            }
            return true;
        }

        void print_stack_trace(std::ostream &out)
        {
            out << "Stack trace (std::stacktrace):\n";

            // best-effort: capturing from a signal handler / SEH filter isn't strictly async-signal-safe
            auto trace = std::stacktrace::current(1, MAX_FRAMES);

            // snippets from testcoe's own files are skipped and the count is capped, so the output
            // stays on the user's code. Reading files here is best-effort too, same as the capture above
            constexpr std::size_t MAX_SNIPPETS = 3;
            std::size_t snippets = 0;

            std::size_t index = 0;
            for (const auto &entry : trace)
            {
                frame f{};
                f.address = to_address(entry.native_handle());
                f.function = entry.description();
                f.file = entry.source_file();
                f.line = static_cast<std::uint32_t>(entry.source_line());
#ifndef _WIN32
                if (f.function.empty())
                    resolve_with_dladdr(f);
#endif

                print_frame(out, index, f);
                if (snippets < MAX_SNIPPETS && !f.file.empty() && f.line > 0 && !is_own_source(f.file) &&
                    print_source_snippet(out, f.file, f.line))
                    ++snippets;
                ++index;
            }
        }

#elif TESTCOE_STACKTRACE_BACKEND_EXECINFO

        void warm_up_stack_trace()
        {
            void *buffer[1];
            backtrace(buffer, 1);
        }

        void print_stack_trace(std::ostream &out)
        {
            out << "Stack trace (execinfo):\n";

            void *buffer[MAX_FRAMES];
            int captured = backtrace(buffer, static_cast<int>(MAX_FRAMES));

            for (int index = 0; index < captured; ++index)
            {
                frame f{};
                f.address = reinterpret_cast<std::uintptr_t>(buffer[index]);
                resolve_with_dladdr(f);
                print_frame(out, static_cast<std::size_t>(index), f);
            }
        }

#else

        void warm_up_stack_trace()
        {
        }

    #if defined(_WIN32) && TESTCOE_STACKTRACE_BACKEND_NONE

        void print_stack_trace(std::ostream &out)
        {
            out << "Stack trace (none):\n";

            void *buffer[MAX_FRAMES];
            USHORT captured = CaptureStackBackTrace(0, static_cast<DWORD>(MAX_FRAMES), buffer, nullptr);

            for (USHORT index = 0; index < captured; ++index)
            {
                frame f{};
                f.address = reinterpret_cast<std::uintptr_t>(buffer[index]);

                HMODULE module = nullptr;
                if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                                       reinterpret_cast<LPCWSTR>(buffer[index]), &module) &&
                    module)
                {
                    char path[MAX_PATH];
                    DWORD len = GetModuleFileNameA(module, path, MAX_PATH);
                    if (len > 0)
                        f.object.assign(path, len);
                }

                print_frame(out, static_cast<std::size_t>(index), f);
            }
        }

    #else

        void print_stack_trace(std::ostream &out)
        {
            out << "stack trace unavailable\n";
        }

    #endif

#endif

    } // namespace internal
} // namespace testcoe
