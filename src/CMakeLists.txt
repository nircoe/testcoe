add_library(testcoe STATIC
    testcoe/grid_listener.cpp
    testcoe/signal_handler.cpp
    testcoe/stack_trace.cpp
    testcoe/terminal_utils.cpp
    testcoe.cpp
)

target_link_libraries(testcoe
    PUBLIC
        gtest_main
        testcoe_headers
        ${TESTCOE_STACKTRACE_LIBS}
)

target_include_directories(testcoe
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${TESTCOE_CONFIG_DIR}
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(testcoe PRIVATE -Wall -Wextra -Werror)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(testcoe PRIVATE /W3 /WX)
endif()

if(TESTCOE_STACKTRACE_BACKEND STREQUAL "execinfo")
    target_link_options(testcoe INTERFACE -rdynamic)
endif()

install(
    TARGETS testcoe
    EXPORT testcoe_targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)