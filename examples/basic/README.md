# Basic testcoe Example

This example demonstrates the basic usage of testcoe to enhance your Google Test output with a visual grid display.

## Files

- `main.cpp` - Main program that initializes testcoe and runs the tests
- `math_tests.cpp` - Test cases for mathematical operations
- `string_tests.cpp` - Test cases for string operations
- `vector_tests.cpp` - Test cases for vector operations

## How to Build

```bash
# Build the example with CMake
mkdir -p build
cd build
cmake ..
cmake --build .
```

## How to Run

```bash
# Run all tests
./basic_example

# Or use the CMake target
cmake --build . --target run_basic_example
```

## Key Points

1. testcoe is initialized with `testcoe::init(&argc, argv)`, replacing the usual Google Test initialization.
2. Tests are run with `testcoe::run()`, replacing the usual `RUN_ALL_TESTS()`.
3. The grid visualization shows all test suites and their status in real-time.
4. An intentional failure is included to demonstrate how failures are displayed.

## Expected Output

You should see a grid display showing the progress of all test suites (MathTest, StringTest, VectorTest) with color-coded status indicators:
- `.` - Not run yet
- `R` - Currently running
- `P` - Passed
- `F` - Failed

When all tests complete, a summary will show the total number of tests, passes, and failures.