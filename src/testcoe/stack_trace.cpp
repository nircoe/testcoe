#include <testcoe/stack_trace.hpp>
#include <testcoe_config.hpp>

#include <cstddef>
#include <ios>

#if TESTCOE_STACKTRACE_BACKEND_STD
#include <stacktrace>
#include <string>
#elif TESTCOE_STACKTRACE_BACKEND_EXECINFO
#if __has_include(<execinfo.h>)
#define TESTCOE_HAS_EXECINFO 1
#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#else
#define TESTCOE_HAS_EXECINFO 0
#endif
#elif defined(_WIN32)
#include <windows.h>
#endif

namespace testcoe
{
    namespace internal
    {
        namespace
        {
            constexpr std::size_t kMaxFrames = 48;

            void print_frame(std::ostream &out, std::size_t index, const frame &f)
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
        } // namespace

#if TESTCOE_STACKTRACE_BACKEND_STD

        void warm_up_stack_trace()
        {
            // first call to std::stacktrace::current() lazily reads debug info (dl_iterate_phdr etc),
            // none of that is async-signal-safe, so do it once here instead of inside the signal handler
            (void)std::to_string(std::stacktrace::current(0, 1));
        }

        void print_stack_trace(std::ostream &out)
        {
            out << "Stack trace (std::stacktrace):\n";

            // best-effort: capturing from a signal handler / SEH filter isn't strictly async-signal-safe
            auto trace = std::stacktrace::current(1, kMaxFrames);

            std::size_t index = 0;
            for (const auto &entry : trace)
            {
                frame f{};
                f.address = static_cast<std::uintptr_t>(entry.native_handle());
                f.function = entry.description();
                f.file = entry.source_file();
                f.line = static_cast<std::uint32_t>(entry.source_line());

                print_frame(out, index, f);
                ++index;
            }
        }

#elif TESTCOE_STACKTRACE_BACKEND_EXECINFO

#if TESTCOE_HAS_EXECINFO

        void warm_up_stack_trace()
        {
            void *buffer[1];
            backtrace(buffer, 1);
        }

        void print_stack_trace(std::ostream &out)
        {
            out << "Stack trace (execinfo):\n";

            void *buffer[kMaxFrames];
            int captured = backtrace(buffer, static_cast<int>(kMaxFrames));

            for (int index = 0; index < captured; ++index)
            {
                Dl_info info{};
                if (dladdr(buffer[index], &info) && info.dli_sname)
                {
                    frame f{};
                    f.address = reinterpret_cast<std::uintptr_t>(buffer[index]);
                    f.object = info.dli_fname ? info.dli_fname : "";

                    int status = 0;
                    char *demangled = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
                    f.function = (status == 0 && demangled) ? demangled : info.dli_sname;
                    std::free(demangled);

                    print_frame(out, static_cast<std::size_t>(index), f);
                }
                else
                {
                    out.flush();
                    std::cerr.flush();
                    backtrace_symbols_fd(&buffer[index], 1, STDERR_FILENO);
                }
            }
        }

#else // musl and other libcs without execinfo.h

        void warm_up_stack_trace()
        {
        }

        void print_stack_trace(std::ostream &out)
        {
            out << "stack trace unavailable\n";
        }

#endif

#elif defined(_WIN32) // TESTCOE_STACKTRACE_BACKEND_NONE

        void warm_up_stack_trace()
        {
        }

        void print_stack_trace(std::ostream &out)
        {
            out << "Stack trace (none):\n";

            void *buffer[kMaxFrames];
            USHORT captured = CaptureStackBackTrace(0, static_cast<DWORD>(kMaxFrames), buffer, nullptr);

            for (USHORT index = 0; index < captured; ++index)
            {
                frame f{};
                f.address = reinterpret_cast<std::uintptr_t>(buffer[index]);
                f.line = 0;

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

#else // TESTCOE_STACKTRACE_BACKEND_NONE preset manually on a non-Windows toolchain

        void warm_up_stack_trace()
        {
        }

        void print_stack_trace(std::ostream &out)
        {
            out << "stack trace unavailable\n";
        }

#endif

    } // namespace internal
} // namespace testcoe
