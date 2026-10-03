#pragma once

#include <ostream>

namespace testcoe
{
    namespace internal
    {
        void warm_up_stack_trace();
        void print_stack_trace(std::ostream &out);
    } // namespace internal
} // namespace testcoe
