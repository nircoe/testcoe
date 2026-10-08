#include <testcoe/stack_trace.hpp>
#include <testcoe_config.hpp>

#include <cstddef>
#include <cstdint>
#include <ios>
#include <string>

#if TESTCOE_STACKTRACE_BACKEND_STD
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
    #define TESTCOE_USE_DLADDR 1
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

#ifdef TESTCOE_USE_DLADDR
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

            [[maybe_unused]] frame resolve_address(std::uintptr_t address)
            {
                frame f{};
                f.address = address;
                resolve_with_dladdr(f);
                return f;
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

        void print_stack_trace(std::ostream &out, std::uintptr_t fault_pc)
        {
            out << "Stack trace (std::stacktrace):\n";

            // best-effort: capturing from a signal handler / SEH filter isn't strictly async-signal-safe
            auto trace = std::stacktrace::current(1, MAX_FRAMES);

            // frames before the faulting one are testcoe's own handler frames, drop them
            std::size_t first = 0;
            bool found = false;
            if (fault_pc != 0)
            {
                for (std::size_t i = 0; i < trace.size(); ++i)
                {
                    // libbacktrace stores address - 1 for frames it doesn't know are signal frames,
                    // and on MinGW the faulting frame looks like a normal one
                    const std::uintptr_t address = to_address(trace[i].native_handle());
                    if (address == fault_pc || address + 1 == fault_pc)
                    {
                        first = i;
                        found = true;
                        break;
                    }
                }
            }

            bool snippet_printed = false;
            for (std::size_t i = first; i < trace.size(); ++i)
            {
                const auto &entry = trace[i];
                frame f{};
                f.address = to_address(entry.native_handle());
                f.function = entry.description();
                f.file = entry.source_file();
                f.line = static_cast<std::uint32_t>(entry.source_line());
#ifdef TESTCOE_USE_DLADDR
                if (f.function.empty())
                    resolve_with_dladdr(f);
#endif

                print_frame(out, i - first, f);
                // without a match on the fault pc, handler frames look like user frames, so no snippet then.
                // reading files here is best-effort too, same as the capture above
                if (found && !snippet_printed && !f.file.empty() && f.line > 0)
                    snippet_printed = print_source_snippet(out, f.file, f.line);
            }
        }

#elif TESTCOE_STACKTRACE_BACKEND_EXECINFO

        void warm_up_stack_trace()
        {
            void *buffer[1];
            backtrace(buffer, 1);
        }

        void print_stack_trace(std::ostream &out, std::uintptr_t fault_pc)
        {
            out << "Stack trace (execinfo):\n";

            void *buffer[MAX_FRAMES];
            int captured = backtrace(buffer, static_cast<int>(MAX_FRAMES));

            std::size_t index = 0;
            int first = 0;
            frame fault{};
            if (fault_pc != 0)
            {
                fault = resolve_address(fault_pc);
                print_frame(out, index++, fault);

                // print_stack_trace, signalHandler and the signal trampoline come first
                constexpr int HANDLER_FRAMES = 3;
                first = HANDLER_FRAMES;
            }

            for (int i = first; i < captured; ++i)
            {
                const frame f = resolve_address(reinterpret_cast<std::uintptr_t>(buffer[i]));

                // the interrupted function shows up again when its frame is walked, skip that copy
                if (i == first && (f.address == fault_pc || (!fault.function.empty() && f.function == fault.function)))
                    continue;

                print_frame(out, index++, f);
            }
        }

#else

        void warm_up_stack_trace()
        {
        }

    #if defined(_WIN32) && TESTCOE_STACKTRACE_BACKEND_NONE

        void print_stack_trace(std::ostream &out, std::uintptr_t fault_pc)
        {
            out << "Stack trace (none):\n";

            void *buffer[MAX_FRAMES];
            USHORT captured = CaptureStackBackTrace(0, static_cast<DWORD>(MAX_FRAMES), buffer, nullptr);

            // frames before the faulting one are testcoe's own handler frames, drop them
            USHORT first = 0;
            if (fault_pc != 0)
            {
                for (USHORT index = 0; index < captured; ++index)
                {
                    if (reinterpret_cast<std::uintptr_t>(buffer[index]) == fault_pc)
                    {
                        first = index;
                        break;
                    }
                }
            }

            for (USHORT index = first; index < captured; ++index)
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

                print_frame(out, static_cast<std::size_t>(index - first), f);
            }
        }

    #else

        void print_stack_trace(std::ostream &out, [[maybe_unused]] std::uintptr_t fault_pc)
        {
            out << "stack trace unavailable\n";
        }

    #endif

#endif

    } // namespace internal
} // namespace testcoe
