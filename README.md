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
- [**backward-cpp**](https://github.com/bombela/backward-cpp) (v1.6) - Stack trace generation

## Quick Start

### 1. Add to your project (CMake)

```cmake
include(FetchContent)
FetchContent_Declare(
    testcoe
    GIT_REPOSITORY https://github.com/nircoe/testcoe.git
    GIT_TAG v0.1.0
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

That's it! Run your tests and see the enhanced output.

## Features

- **Grid Visualization** - See all tests progress in real-time
- **Crash Handling** - Get stack traces when tests crash
- **Color Support** - Automatic terminal detection
- **Test Filtering** - Run specific tests or suites
- **Zero Config** - Works out of the box with Google Test

## Examples

See the [examples/](examples/) directory for demonstrations:
- [Basic usage](examples/basic/) - Grid visualization
- [Crash handling](examples/crash/) - Stack traces on crashes
- [Test filtering](examples/filter/) - Running specific tests

## Requirements

- C++17 or later
- CMake 3.14+
- Google Test (automatically included)

## API Reference

```cpp
// Initialize testcoe
testcoe::init(&argc, argv);

// Run all tests
testcoe::run();

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