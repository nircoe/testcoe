add_library(testcoe
    grid_listener.cpp
    signal_handler.cpp
    terminal_utils.cpp
    testcoe.cpp
)

if(TARGET GTest::Main)
    set(GTEST_TARGET GTest::Main)
else()
    set(GTEST_TARGET gtest_main)
endif()

if(TARGET Backward::Backward)
    set(BACKWARD_TARGET Backward::Backward)
else()
    set(BACKWARD_TARGET backward)
endif()

target_link_libraries(testcoe
    PRIVATE
        ${GTEST_TARGET}
        ${BACKWARD_TARGET}
    PUBLIC
        testcoe_headers
)

target_include_directories(testcoe
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(testcoe PRIVATE -Wall -Wextra -Werror)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(testcoe PRIVATE /W3 /WX)
endif()

install(
    TARGETS testcoe
    EXPORT testcoe_targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)