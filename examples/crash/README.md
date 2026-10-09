# testcoe Crash Handling Example

This example demonstrates testcoe's crash handling capabilities, showing how it detects crashes and provides
detailed stack traces.

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

By default only the non-crashing `BasicTests` run. The `CrashTests` suite is excluded:

```bash
./examples/crash/crash_example
```

To run crash tests, pass one of these flags as the first argument. `--gtest_filter` has no effect here,
because `main.cpp` always runs with its own filter.

- `--run-segfault` - `CrashTests.SegmentationFault`
- `--run-abort` - `CrashTests.Abort`
- `--run-divbyzero` - `CrashTests.DivideByZero`
- `--run-crash-suite` - the whole `CrashTests` suite
- `--run-death-test` - `CrashTests.AbortDeathTest`

```bash
# Run only the segmentation fault test
./examples/crash/crash_example --run-segfault
```

The process ends at the first crash, so `--run-crash-suite` stops at `SegmentationFault`.

Or use the provided CMake targets:

```bash
# Run only the divide by zero test
cmake --build . --target run_crash_example_DivideByZero
```

The targets are `run_crash_example_SegmentationFault`, `run_crash_example_DivideByZero`,
`run_crash_example_Abort` and `run_crash_example_AbortDeathTest`.

## Available Crash Tests

- `SegmentationFault` - Demonstrates handling of null pointer dereference
- `DivideByZero` - Demonstrates handling of division by zero
- `Abort` - Demonstrates handling of program abort
- `StackOverflow` - Demonstrates handling of stack overflow
- `OutOfBounds` - Demonstrates handling of out-of-bounds access
- `StreamRedirection` - Demonstrates handling of crashes with redirected streams
- `AbortDeathTest` - Regression test for gtest's `EXPECT_DEATH` under testcoe (never skipped, only runs with
  `--run-death-test`)

`StackOverflow`, `OutOfBounds` and `StreamRedirection` skip themselves with `GTEST_SKIP()` and have no flag yet.

## Death Test Regression Check

Unlike the crash tests above, `AbortDeathTest` doesn't crash the whole binary. It uses gtest's
`EXPECT_DEATH` to assert a child process aborts as expected, and must pass cleanly:

```bash
./examples/crash/crash_example --run-death-test
```

Guards against a signal-handler/death-test collision on Windows: testcoe must not intercept the
child's abort before gtest's own death-test protocol can observe it.

## Running a Test Without a Flag

To run `StackOverflow`, `OutOfBounds` or `StreamRedirection`:
1. Remove the `GTEST_SKIP()` line in the test in `crash_tests.cpp`
2. Call `testcoe::run_test("CrashTests", "<Name>")` from `main.cpp`

## Key Points

1. testcoe catches and reports various types of crashes
2. When a crash occurs, testcoe:
   - Displays the type of signal that occurred
   - Shows a detailed stack trace
   - Provides helpful debugging information
3. testcoe restores output streams even when a crash occurs with redirected streams
4. Trace detail depends on the platform backend:
   - File:line and source snippets with the std::stacktrace backend
   - Function names and addresses with the fallback (macOS)