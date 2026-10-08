#pragma once

#include <cstdint>
#include <ostream>

namespace testcoe
{
    namespace internal
    {
        void warm_up_stack_trace();
        void print_stack_trace(std::ostream &out, std::uintptr_t fault_pc = 0);
    } // namespace internal
} // namespace testcoe
