function(testcoe_fetch_googletest)
    if(TARGET gtest_main)
        message(STATUS "[testcoe] googletest already available, skipping fetch")
        return()
    endif()

    message(STATUS "[testcoe] Fetching googletest from source...")

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.16.0
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(googletest)
endfunction()
