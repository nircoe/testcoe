# testcoe Roadmap

## Version History

### v0.1.0 - Initial Release
- ✅ Grid-based visualization for test execution
- ✅ Real-time test progress display
- ✅ Enhanced crash handling with stack traces
- ✅ Cross-platform support (Windows, Linux, macOS)
- ✅ Test filtering API (run_suite, run_test)
- ✅ Terminal ANSI color support detection
- ✅ Comprehensive examples and integration tests
- ✅ Support for MSVC, GCC, Clang, and MinGW compilers

### v0.1.1 - CMake 4 Compatibility
- Fix configure errors with CMake 4.0+ by setting `CMAKE_POLICY_VERSION_MINIMUM`

### v0.1.2 - MSVC Cache Compatibility
- Compile MSVC builds with `/Z7` instead of `/Zi` so sccache/ccache work with parallel builds

### v0.2.0 - std::stacktrace
- Replace backward-cpp with `std::stacktrace`, falling back to `<execinfo.h>` where it is unavailable
- Stack trace backend picked at configure time (`TESTCOE_STACKTRACE_BACKEND` to override)
- Source snippets around crash trace frames
- Crash traces start at the faulting frame
- Require C++23
- Fix the signal handler breaking GoogleTest death tests on Windows
- Fix duplicated test output

## Future Plans

- ⏳ Customizable grid layout and colors
- ⏳ JSON/XML test report generation
- ⏳ Test execution time tracking and reporting

## Feature Requests

Have an idea for testcoe? Please open an issue on GitHub with the "enhancement" label.
And of course, feel free to reach out at nircoe@gmail.com

## Versioning

testcoe follows [Semantic Versioning](https://semver.org/):
- MAJOR version for incompatible API changes
- MINOR version for backwards-compatible functionality additions
- PATCH version for backwards-compatible bug fixes