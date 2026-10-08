# Detects a working stack trace backend and derives TESTCOE_STACKTRACE_* variables.
# TESTCOE_STACKTRACE_BACKEND (cache) is only an override, the detected values are plain variables.
include(CheckCXXSourceCompiles)

# Tries each library in the list named by candidates_var until the source links.
# Sets out_found and out_lib in the caller's scope.
function(testcoe_probe_libs source prefix candidates_var out_found out_lib)
    # Each attempt needs its own result var, check_cxx_source_compiles caches it,
    # so reusing one across attempts would just return the old cached result
    # instead of retrying with the next CMAKE_REQUIRED_LIBRARIES.
    foreach(_lib IN LISTS ${candidates_var})
        string(MAKE_C_IDENTIFIER "${prefix}_${_lib}" _result_var)
        set(CMAKE_REQUIRED_LIBRARIES "${_lib}")
        check_cxx_source_compiles("${source}" ${_result_var})
        if(${_result_var})
            set(${out_found} TRUE PARENT_SCOPE)
            set(${out_lib} "${_lib}" PARENT_SCOPE)
            return()
        endif()
    endforeach()

    set(${out_found} FALSE PARENT_SCOPE)
    set(${out_lib} "" PARENT_SCOPE)
endfunction()

function(testcoe_detect_stacktrace)
    set(TESTCOE_STACKTRACE_BACKEND "" CACHE STRING "override: std, execinfo or none (empty = auto)")
    set(_preset "$CACHE{TESTCOE_STACKTRACE_BACKEND}")
    if(NOT _preset STREQUAL "" AND NOT _preset MATCHES "^(std|execinfo|none)$")
        message(FATAL_ERROR "[testcoe] TESTCOE_STACKTRACE_BACKEND must be std, execinfo or none, got \"${_preset}\"")
    endif()

    # Link probe, not a __cpp_lib_stacktrace check: GCC defines that macro even when
    # the backtrace lib isn't linked (checked on GCC 16), so a header-only "success"
    # would slip through. std::to_string(s) forces the backtrace symbols to actually
    # get pulled in.
    set(_std_probe_src [[
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

    set(_execinfo_probe_src [[
#include <execinfo.h>
#include <dlfcn.h>

int main() {
    void *buffer[4];
    int captured = backtrace(buffer, 4);
    Dl_info info{};
    return (captured > 0 && dladdr(buffer[0], &info)) ? 0 : 1;
}
]])

    # A preset skips the other probe, and execinfo is only probed as the fallback when std didn't link.
    set(_std_found FALSE)
    set(_std_lib "")
    if(NOT _preset OR _preset STREQUAL "std")
        set(_std_candidates "" "stdc++exp" "stdc++_libbacktrace")
        testcoe_probe_libs("${_std_probe_src}" TESTCOE_STACKTRACE_COMPILES _std_candidates _std_found _std_lib)
    endif()

    set(_execinfo_found FALSE)
    set(_execinfo_lib "")
    if(NOT WIN32 AND ((NOT _preset AND NOT _std_found) OR _preset STREQUAL "execinfo"))
        set(_execinfo_candidates "" "execinfo")
        if(CMAKE_DL_LIBS)
            list(APPEND _execinfo_candidates "${CMAKE_DL_LIBS}")
        endif()
        testcoe_probe_libs("${_execinfo_probe_src}" TESTCOE_EXECINFO_COMPILES _execinfo_candidates _execinfo_found _execinfo_lib)
    endif()

    if(_preset STREQUAL "std" AND NOT _std_found)
        message(FATAL_ERROR "[testcoe] TESTCOE_STACKTRACE_BACKEND is std but the std::stacktrace probe failed")
    elseif(_preset STREQUAL "execinfo" AND NOT _execinfo_found)
        message(FATAL_ERROR "[testcoe] TESTCOE_STACKTRACE_BACKEND is execinfo but the execinfo probe failed")
    endif()

    if(_preset)
        set(_backend "${_preset}")
    elseif(_std_found)
        set(_backend "std")
    elseif(_execinfo_found)
        set(_backend "execinfo")
    else()
        set(_backend "none")
    endif()

    set(_libs "")
    if(_backend STREQUAL "std")
        if(_std_lib)
            list(APPEND _libs "${_std_lib}")
        endif()
        # the std backend resolves unknown frames with dladdr
        if(NOT WIN32)
            list(APPEND _libs ${CMAKE_DL_LIBS})
        endif()
    elseif(_backend STREQUAL "execinfo" AND _execinfo_lib)
        list(APPEND _libs "${_execinfo_lib}")
    endif()

    set(_note "")
    if(_libs)
        list(JOIN _libs ", " _libs_text)
        set(_note " (+${_libs_text})")
    endif()
    if(_preset)
        set(_note "${_note} (preset)")
    endif()
    message(STATUS "[testcoe] Stack trace backend: ${_backend}${_note}")
    message(STATUS "[testcoe] To change: \"set(TESTCOE_STACKTRACE_BACKEND <std|execinfo|none> CACHE STRING \"\")\" before fetching testcoe")

    set(TESTCOE_STACKTRACE_BACKEND "${_backend}" PARENT_SCOPE)
    set(TESTCOE_STACKTRACE_LIBS "${_libs}" PARENT_SCOPE)

    foreach(_name std execinfo none)
        string(TOUPPER "${_name}" _upper)
        if(_name STREQUAL _backend)
            set(TESTCOE_STACKTRACE_BACKEND_${_upper} 1 PARENT_SCOPE)
        else()
            set(TESTCOE_STACKTRACE_BACKEND_${_upper} 0 PARENT_SCOPE)
        endif()
    endforeach()
endfunction()
