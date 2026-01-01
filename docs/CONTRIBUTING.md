# Contributing to testcoe

Thank you for your interest in contributing to testcoe!

## Development Setup

### Prerequisites
- CMake 3.14+
- C++17 compatible compiler
- Git

### Building from Source

```bash
git clone https://github.com/nircoe/testcoe.git
cd testcoe
mkdir build && cd build
cmake -DBUILD_EXAMPLES=ON -DBUILD_TESTS=ON ..
cmake --build .
```

### Running Tests

```bash
# Run unit tests
./tests/testcoe_tests

# Run examples
./examples/basic/basic_example
./examples/crash/crash_example
./examples/filter/filter_example
```

## Project Structure

```
testcoe/
├── include/testcoe/     # Public headers
├── src/                 # Implementation files
├── examples/            # Example programs
├── tests/              # Integration and unit tests
└── .github/workflows/  # CI configuration
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
3. Run integration tests
4. Verify examples execute correctly

## Making Changes

### Code Style
- Follow existing naming conventions
- Keep lines under 120 characters
- Add comments for complex logic

### Testing
- Add tests for new features
- Ensure all tests pass locally
- Test on multiple platforms if possible

### Pull Request Process

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Run tests locally
5. Commit with clear messages
6. Push to your fork
7. Open a Pull Request from your fork to the main repository

### Commit Messages
- Use prefix for PR title `[Subject]: <PR title>`
- PR description should describe the major changes in bullet-points
- Sqaushed commit title should be the PR title, and the message should be PR description

## Adding New Features

When adding features:
1. Update the public API in `include/testcoe/testcoe.hpp` if needed
2. Add implementation in appropriate source file
3. Create example demonstrating the feature
4. Add tests covering the new functionality
5. Update documentation

## Questions?

Feel free to reach out at nircoe@gmail.com

I'm here to help make contributing to logcoe as smooth as possible!

Please open an issue for:
- Bug reports
- Feature requests
- Questions about the codebase
- Discussion about implementation