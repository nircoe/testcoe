cmake_minimum_required(VERSION 3.14)
project(testcoe_basic_example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
# Find testcoe package or fetch it from GitHub
  find_package(testcoe QUIET)
  if(NOT testcoe_FOUND)
    include(FetchContent)
    FetchContent_Declare(
      testcoe
      GIT_REPOSITORY https://github.com/nircoe/testcoe.git
      GIT_TAG main # use v0.1.0 later on
    )
    FetchContent_MakeAvailable(testcoe)
  endif()
endif()

# Note: You don't need to find or fetch GTest separately
# because testcoe already includes and links against it.

# Add the executable
add_executable(basic_example
    main.cpp
    math_tests.cpp
    string_tests.cpp
    vector_tests.cpp
)

# Link against testcoe
target_link_libraries(basic_example
    PRIVATE
        testcoe
)

# Add an easy way to run the example
add_custom_target(run_basic_example
    COMMAND basic_example
    DEPENDS basic_example
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)