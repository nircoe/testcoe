# Detects a working std::stacktrace backend and derives TESTCOE_STACKTRACE_* variables.
function(testcoe_detect_stacktrace)
    if(DEFINED CACHE{TESTCOE_STACKTRACE_BACKEND})
        # Respect a preset value (the documented override/escape hatch) instead of re-probing.
        message(STATUS "[testcoe] Stack trace backend: ${TESTCOE_STACKTRACE_BACKEND} (preset)")
    else()
        include(CheckCXXSourceCompiles)

        # Link probe, not a __cpp_lib_stacktrace check: GCC defines that macro even when
        # the backtrace lib isn't linked (checked on GCC 16), so a header-only "success"
        # would slip through. std::to_string(s) forces the backtrace symbols to actually
        # get pulled in.
        set(_TESTCOE_STACKTRACE_PROBE_SRC [[
#include <stacktrace>
#include <string>

#ifndef __cpp_lib_stacktrace
#error "std::stacktrace not available"
#endif

int main() {
    auto s = std::stacktrace::current();
    return std::to_string(s).empty() ? 1 : 0;
}
]])

        set(_TESTCOE_STACKTRACE_BACKEND "")
        set(_TESTCOE_STACKTRACE_LIBS "")

        # Each attempt needs its own result var, check_cxx_source_compiles caches it,
        # so reusing one across attempts would just return the old cached result
        # instead of retrying with the next CMAKE_REQUIRED_LIBRARIES.
        foreach(_lib "" "stdc++exp" "stdc++_libbacktrace")
            string(MAKE_C_IDENTIFIER "TESTCOE_STACKTRACE_COMPILES_${_lib}" _result_var)
            set(CMAKE_REQUIRED_LIBRARIES "${_lib}")
            check_cxx_source_compiles("${_TESTCOE_STACKTRACE_PROBE_SRC}" ${_result_var})
            if(${_result_var})
                set(_TESTCOE_STACKTRACE_BACKEND "std")
                set(_TESTCOE_STACKTRACE_LIBS "${_lib}")
                break()
            endif()
        endforeach()

        set(CMAKE_REQUIRED_LIBRARIES "")

        if(NOT _TESTCOE_STACKTRACE_BACKEND)
            if(WIN32)
                set(_TESTCOE_STACKTRACE_BACKEND "none")
            else()
                set(_TESTCOE_STACKTRACE_BACKEND "execinfo")
            endif()
        endif()

        set(TESTCOE_STACKTRACE_BACKEND "${_TESTCOE_STACKTRACE_BACKEND}" CACHE STRING "testcoe stack trace backend (std, execinfo, or none)")
        set(TESTCOE_STACKTRACE_BACKEND "${_TESTCOE_STACKTRACE_BACKEND}")
        set(TESTCOE_STACKTRACE_LIBS "${_TESTCOE_STACKTRACE_LIBS}")

        if(_TESTCOE_STACKTRACE_LIBS)
            message(STATUS "[testcoe] Stack trace backend: ${_TESTCOE_STACKTRACE_BACKEND} (+${_TESTCOE_STACKTRACE_LIBS})")
        else()
            message(STATUS "[testcoe] Stack trace backend: ${_TESTCOE_STACKTRACE_BACKEND}")
        endif()
        message(STATUS "[testcoe] To change: \"set(TESTCOE_STACKTRACE_BACKEND <std|execinfo|none> CACHE STRING \"\")\" before fetching testcoe")
    endif()

    set(TESTCOE_STACKTRACE_BACKEND "${TESTCOE_STACKTRACE_BACKEND}" PARENT_SCOPE)
    set(TESTCOE_STACKTRACE_LIBS "${TESTCOE_STACKTRACE_LIBS}" PARENT_SCOPE)

    set(_active NONE)
    if(TESTCOE_STACKTRACE_BACKEND MATCHES "^(std|execinfo)$")
        string(TOUPPER "${TESTCOE_STACKTRACE_BACKEND}" _active)
    endif()
    foreach(_name STD EXECINFO NONE)
        if(_name STREQUAL _active)
            set(TESTCOE_STACKTRACE_BACKEND_${_name} 1 PARENT_SCOPE)
        else()
            set(TESTCOE_STACKTRACE_BACKEND_${_name} 0 PARENT_SCOPE)
        endif()
    endforeach()
endfunction()
