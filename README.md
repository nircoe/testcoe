# testcoe

Visual test progress and crash handling for Google Test.

[![Windows](https://github.com/nircoe/testcoe/actions/workflows/ci-windows.yml/badge.svg)](https://github.com/nircoe/testcoe/actions/workflows/ci-windows.yml)
[![Linux](https://github.com/nircoe/testcoe/actions/workflows/ci-linux.yml/badge.svg)](https://github.com/nircoe/testcoe/actions/workflows/ci-linux.yml)
[![macOS](https://github.com/nircoe/testcoe/actions/workflows/ci-macos.yml/badge.svg)](https://github.com/nircoe/testcoe/actions/workflows/ci-macos.yml)

## What is testcoe?

testcoe enhances Google Test with real-time grid visualization and crash reporting:

```
Running 12 tests... Completed: 8/12 (P: 6, F: 2)
P - Passed, F - Failed, R - Running, . - Not run yet

MathTests    PPF.. (3/5)
StringTests  PRPP. (4/5)
VectorTests  ....  (0/2)
```

When tests crash, you get detailed stack traces instead of silent failures.

## Dependencies
- [**Google Test**](https://github.com/google/googletest) (v1.16.0) - Test framework

## Quick Start

### 1. Add to your project (CMake)

```cmake
include(FetchContent)
FetchContent_Declare(
    testcoe
    GIT_REPOSITORY https://github.com/nircoe/testcoe.git
    GIT_TAG v0.2.0
)
FetchContent_MakeAvailable(testcoe)

target_link_libraries(your_test_executable PRIVATE testcoe)
```

### 2. Use in your tests

```cpp
#include <testcoe.hpp>

int main(int argc, char** argv) {
    testcoe::init(&argc, argv);
    return testcoe::run();
}
```

Run your tests and see the enhanced output.

## Features

- Grid visualization: real-time progress on interactive terminals. When output is piped or redirected
  (e.g. CI logs), a single summary is printed instead.
- Crash handling: stack traces when tests crash
- Color support: automatic terminal detection
- Test filtering: run specific tests or suites
- Zero config: works out of the box with Google Test

## Examples

See the [examples/](examples/) directory for demonstrations:
- [Basic usage](examples/basic/) - Grid visualization
- [Crash handling](examples/crash/) - Stack traces on crashes
- [Test filtering](examples/filter/) - Running specific tests

## Requirements

- C++23 or later
- CMake 3.22+
- Google Test (automatically included)
- Stack traces use `std::stacktrace` where the toolchain supports it (GCC 13+, MSVC), or
  `<execinfo.h>` otherwise (macOS).
- File:line and source snippets in crash reports need the `std::stacktrace` backend. The fallback
  shows function names and addresses.

## API Reference

```cpp
// Initialize testcoe
testcoe::init(&argc, argv);

// Initialize without arguments (uses Google Test's own init)
testcoe::init();

// Check if testcoe was initialized
testcoe::isInitialized();

// Run all tests
testcoe::run();

// Run tests matching a Google Test filter string
testcoe::run("MathTests.*");

// Run specific suite
testcoe::run_suite("MathTests");

// Run specific test
testcoe::run_test("MathTests", "Addition");
```

## Documentation

- [Architecture](docs/ARCHITECTURE.md) - How testcoe works internally
- [Contributing](docs/CONTRIBUTING.md) - Development setup and guidelines
- [Roadmap](docs/ROADMAP.md) - Version history and future plans

## License

MIT License - see [LICENSE](LICENSE) file for details.