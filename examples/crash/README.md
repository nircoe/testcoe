# testcoe Crash Handling Example

This example demonstrates testcoe's crash handling capabilities, showing how it detects crashes and provides detailed stack traces.

## Files

- `main.cpp` - Main program that initializes testcoe and runs the tests
- `crash_tests.cpp` - Various tests demonstrating different types of crashes

## How to Build

```bash
# Build using CMake
mkdir -p build
cd build
cmake ..
cmake --build .
```

## How to Run

By default, all crash-causing tests are skipped to prevent unintentional crashes:

```bash
# Run the example with all tests (non-crashing)
./examples/crash/crash_example
```

To run a specific crash test, use the Google Test filter mechanism:

```bash
# Run only the segmentation fault test
./examples/crash/crash_example --gtest_filter=CrashTests.SegmentationFault
```

Or use the provided CMake targets:

```bash
# Run only the divide by zero test
cmake --build . --target run_crash_example_DivideByZero
```

## Available Crash Tests

- `SegmentationFault` - Demonstrates handling of null pointer dereference
- `DivideByZero` - Demonstrates handling of division by zero
- `Abort` - Demonstrates handling of program abort
- `StackOverflow` - Demonstrates handling of stack overflow
- `OutOfBounds` - Demonstrates handling of out-of-bounds access
- `StreamRedirection` - Demonstrates handling of crashes with redirected streams

## Enabling a Crash Test

To enable a crash test, edit `crash_tests.cpp` and:
1. Comment out the `GTEST_SKIP()` line in the test you want to run
2. Uncomment the crash-causing code

## Key Points

1. testcoe catches and reports various types of crashes
2. When a crash occurs, testcoe:
   - Displays the type of signal that occurred
   - Shows a detailed stack trace
   - Provides helpful debugging information
3. testcoe restores output streams even when a crash occurs with redirected streams