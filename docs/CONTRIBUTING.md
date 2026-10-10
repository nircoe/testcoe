# Contributing to testcoe

## Development Setup

### Prerequisites
- CMake 3.22+
- C++23 compatible compiler

### Building from Source

```bash
git clone https://github.com/nircoe/testcoe.git
cd testcoe
mkdir build && cd build
cmake -DTESTCOE_BUILD_EXAMPLES=ON -DTESTCOE_BUILD_TESTS=ON ..
cmake --build .
```

When fetching testcoe from another CMake project, set the same options with
`set(TESTCOE_BUILD_EXAMPLES ON)` / `set(TESTCOE_BUILD_TESTS ON)` before FetchContent_MakeAvailable
(tests require examples ON).

### Running Tests

```bash
# Run the integration tests
./tests/testcoe_tests

# Run examples
./examples/basic/basic_example
./examples/crash/crash_example
./examples/filter/filter_example
```

## Project Structure

```
testcoe/
├── cmake/                # CMake helpers
├── docs/                 # Architecture, contributing and roadmap
├── include/testcoe/      # Public headers
├── src/                  # Implementation files
├── examples/             # Example programs
├── tests/                # Integration tests (they run the example binaries)
└── .github/workflows/    # CI configuration
```

## Continuous Integration

All pull requests are automatically tested on:

- **Windows**: MSVC and MinGW
- **Linux**: GCC and Clang
- **macOS**: Apple Clang

### CI Pipeline Details

The CI runs the following checks:
1. Build the library
2. Build all examples
3. Run the integration tests (they run the example binaries)

## Making Changes

### Code Style
- Keep lines under 120 characters

### Testing
- The stack trace backend is chosen at configure time (look for `[testcoe] Stack trace backend:`
  in CMake output). Override via `set(TESTCOE_STACKTRACE_BACKEND "execinfo" CACHE STRING "")`
  before fetching, or `-DTESTCOE_STACKTRACE_BACKEND=...` standalone.

### Pull Request Process

Open a PR from a feature branch against `main`. CI must pass.

### Commit Messages
- Use prefix for PR title `[Subject]: <PR title>`
- PR description should describe the major changes in bullet-points
- Squashed commit title should be the PR title, and the message should be PR description

## Adding New Features

When adding features:
1. Update the public API in `include/testcoe.hpp` if needed
2. Add implementation in appropriate source file
3. Create example demonstrating the feature
4. Add tests covering the new functionality
5. Update documentation

## Questions?

Feel free to reach out at nircoe@gmail.com

Open an issue on GitHub for bug reports, feature requests or questions.