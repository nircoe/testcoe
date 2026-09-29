#pragma once

#include <cstdint>
#include <ostream>
#include <string>

namespace testcoe
{
    namespace internal
    {
        struct frame
        {
            std::uintptr_t address;
            std::string object;
            std::string function;
            std::string file;
            std::uint32_t line;
        };

        void warm_up_stack_trace();
        void print_stack_trace(std::ostream &out);
    } // namespace internal
} // namespace testcoe
