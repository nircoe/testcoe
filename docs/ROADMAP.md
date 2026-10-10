# testcoe Roadmap

## Version History

### v0.2.0 - std::stacktrace
- Replace backward-cpp with `std::stacktrace`, falling back to `<execinfo.h>` where it is unavailable
- Stack trace backend picked at configure time (`TESTCOE_STACKTRACE_BACKEND` to override)
- Source snippets around crash trace frames
- Crash traces start at the faulting frame
- Require C++23
- Fix the signal handler breaking GoogleTest death tests on Windows
- Fix duplicated test output

### [v0.1.2 - MSVC Cache Compatibility](https://github.com/nircoe/testcoe/releases/tag/v0.1.2)

### [v0.1.1 - CMake 4 Compatibility](https://github.com/nircoe/testcoe/releases/tag/v0.1.1)

### [v0.1.0 - Initial Release](https://github.com/nircoe/testcoe/releases/tag/v0.1.0)

## Future Plans

- Customizable grid layout and colors
- JSON/XML test report generation
- Test execution time tracking and reporting

## Feature Requests

Have an idea for testcoe? Please open an issue on GitHub with the "enhancement" label.
And of course, feel free to reach out at nircoe@gmail.com

## Versioning

testcoe follows [Semantic Versioning](https://semver.org/):
- MAJOR version for incompatible API changes
- MINOR version for backwards-compatible functionality additions
- PATCH version for backwards-compatible bug fixes