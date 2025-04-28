# testcoe Test Filtering Example

This example demonstrates how to use testcoe's test filtering capabilities to run specific test suites or individual tests.

## Files

- `main.cpp` - Main program that initializes testcoe and handles test filtering
- `test_suites.cpp` - Multiple test suites for demonstration

## How to Build

```bash
# Build using CMake
mkdir -p build
cd build
cmake ..
cmake --build .
```

## How to Run

### Run all tests:
```bash
./filtering_example --all
```

### Run a specific test suite:
```bash
./filtering_example --suite=MathSuite
```

### Run a specific test:
```bash
./filtering_example --test=StringSuite.Length
```

### Show help message:
```bash
./filtering_example --help
```

## Test Suites Available

The example contains three test suites:

1. **MathSuite** - Basic mathematical operations:
   - Addition
   - Subtraction
   - Multiplication
   - Division

2. **StringSuite** - String operations:
   - Length
   - Concatenation
   - Comparison

3. **VectorSuite** - Vector operations:
   - Size
   - PushBack
   - Clear

## Key Points

1. testcoe provides a simple API for running specific tests:
   - `testcoe::run()` - Run all tests
   - `testcoe::run_suite("SuiteName")` - Run a specific test suite
   - `testcoe::run_test("SuiteName", "TestName")` - Run a specific test

2. This example demonstrates how to build a command-line interface around these methods

3. The grid visualization adapts to show only the tests that are being run