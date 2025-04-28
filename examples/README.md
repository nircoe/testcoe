# testcoe Examples

This directory contains examples demonstrating how to use testcoe in your projects.

## Directory Structure

```
examples/
├── basic/                    # Basic usage example
│   ├── CMakeLists.txt
│   ├── main.cpp              # Main program initializing testcoe
│   ├── math_tests.cpp        # Mathematical test cases
│   ├── string_tests.cpp      # String operation test cases
│   ├── vector_tests.cpp      # Vector operation test cases
│   └── README.md             # Example-specific instructions
│
├── crash_handling/           # Crash handling example
│   ├── CMakeLists.txt
│   ├── main.cpp              # Main program initializing testcoe
│   ├── crash_tests.cpp       # Various crash test cases
│   └── README.md             # Example-specific instructions
│
├── filtering/                # Test filtering example
│   ├── CMakeLists.txt
│   ├── main.cpp              # Main program with filtering options
│   ├── test_suites.cpp       # Multiple test suites
│   └── README.md             # Example-specific instructions
│
├── CMakeLists.txt            # Main CMake file for all examples
└── README.md                 # This file
```

## Building All Examples

To build all examples at once:

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
```

Or build a specific example:

```bash
cmake --build . --target basic_example
```

## Running the Examples

Each example can be run individually. See the README.md file in each example
directory for specific instructions.

```bash
# Run the basic example
./basic/basic_example

# Run the crash handling example
./crash_handling/crash_example

# Run the filtering example
./filtering/filtering_example
```

## Key Concepts Demonstrated

1. **Basic Example** - Shows how to use testcoe's grid visualization with multiple test files.

2. **Crash Handling Example** - Demonstrates testcoe's ability to catch crashes and provide detailed stack traces.

3. **Filtering Example** - Shows how to use testcoe's API to run specific test suites or individual tests.

## Getting Started

If you're new to testcoe, start with the Basic Example to understand the core functionality, then explore the other examples to learn about additional features.

## Integration into Your Project

To use testcoe in your own project, you only need to:

1. Add testcoe to your project (via CMake's FetchContent or as a dependency)
2. Initialize it with `testcoe::init()`
3. Run tests with `testcoe::run()` (or the other run methods)

Note that testcoe already includes and links against Google Test, so you don't need to find or fetch GTest separately.