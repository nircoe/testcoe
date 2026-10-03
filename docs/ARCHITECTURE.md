# testcoe Architecture

## Overview

testcoe enhances Google Test by intercepting test events and providing visual feedback. It consists of several modular components working together.

## Component Architecture

```
                                          ┌─────────────────────────────────────┐
                                          │          Test Application           │
                                          │         (Your Google Tests)         │
                                          └──────────────────┬──────────────────┘
                                                             │
                                          ┌──────────────────▼──────────────────┐
                                          │               testcoe               │
                                          │       (Main Interface & API)        │
                                          └──────────────────┬──────────────────┘
                                                             │
                                          ┌──────────────────│──────────────────┐
                                          │                  │                  │
                                  ┌───────▼───────┐   ┌──────▼──────┐    ┌──────▼──────┐
                                  │ GridListener  │   │SignalHandler│    │TerminalUtils│
                                  │ (Visual Grid) │   │  (Crashes)  │    │  (Terminal) │
                                  └───────────────┘   └──────┬──────┘    └─────────────┘
                                                             │
                                                   ┌─────────▼─────────┐
                                                   │    stack_trace    │
                                                   │    (internal)     │
                                                   └───────────────────┘
```

## Core Components

### testcoe (Main Interface)
- **File**: `src/testcoe.cpp`, `include/testcoe.hpp`
- **Purpose**: Provides the public API for initialization and test execution
- **Key Functions**:
  - `init()` - Initializes Google Test and installs custom listeners
  - `run()` - Executes all tests
  - `run_suite()` - Executes specific test suite
  - `run_test()` - Executes specific test

### GridListener
- **File**: `src/testcoe/grid_listener.cpp`, `include/testcoe/grid_listener.hpp`
- **Purpose**: Implements Google Test event listener for visual grid display
- **Key Features**:
  - Tracks test execution state
  - Updates terminal display in real-time on interactive terminals; when output is piped or redirected (e.g. CI logs), redraws only once, in the final summary, to avoid duplicate frames
  - Collects and displays failure information
  - Shows execution time statistics

### SignalHandler
- **File**: `src/testcoe/signal_handler.cpp`, `include/testcoe/signal_handler.hpp`
- **Purpose**: Catches crashes and provides detailed stack traces
- **Platform Support**:
  - **Unix/Linux/macOS**: POSIX signal handlers (SIGSEGV, SIGABRT, etc.)
  - **Windows**: Structured Exception Handling (SEH)
- Trace capture and printing is done by the internal stack_trace module (backend picked at
  configure time, see Stack traces below).

### Stack traces (internal)
- **Files**: `include/testcoe/stack_trace.hpp`, `src/testcoe/stack_trace.cpp` (namespace
  `testcoe::internal`)
- **Purpose**: Captures and formats stack traces on crash
- **Frame line format**: `#N 0xADDR in <function> at <file>:<line> [<object>]` with empty
  fields dropped. Plain text, no ANSI colors.
- **Backend Selection**:
  The backend is chosen at CMake configure time by a link probe (`cmake/stacktrace.cmake`) and
  logged as `[testcoe] Stack trace backend: <name>`. Override via cache variable
  `TESTCOE_STACKTRACE_BACKEND` (values: std, execinfo, none).

| Backend | Chosen when | Output |
|---------|-------------|--------|
| `std` | GCC 13+, MSVC, anything where `std::stacktrace` links | function, file:line, source snippets (+-2 lines around the crash line) for at most 3 frames, skipping testcoe's own sources |
| `execinfo` | probe fails on non-Windows (macOS, Apple libc++ has no `<stacktrace>`) | addresses, object, function names via `dladdr`. No file:line or snippets |
| `none` | probe fails on Windows | addresses and module only, via `CaptureStackBackTrace` |

### TerminalUtils
- **File**: `src/testcoe/terminal_utils.cpp`, `include/testcoe/terminal_utils.hpp`
- **Purpose**: Cross-platform terminal operations
- **Key Functions**:
  - `isAnsiEnabled()` - Detects ANSI color support
  - `clear()` - Clears terminal screen
  - `isInteractive()` - Detects whether stdout is an interactive terminal (TTY) vs piped/redirected

## Data Flow

1. **Initialization**:
   - User calls `testcoe::init()`
   - Google Test is initialized
   - GridListener is installed
   - Signal handlers are registered
   - A one-time warm-up capture runs to initialize stack trace state

2. **Test Execution**:
   - User calls `testcoe::run()`
   - Google Test begins execution
   - GridListener receives events:
     - `OnTestProgramStart` - Initialize grid display
     - `OnTestStart` - Mark test as running (redraws the grid only when output is interactive)
     - `OnTestEnd` - Mark test as passed/failed (redraws the grid only when output is interactive)
     - `OnTestProgramEnd` - Show final summary

3. **Crash Handling**:
   - If a test crashes, signal handler is triggered
   - Stack trace is generated by the stack_trace module
   - Output streams are restored
   - Detailed crash report is displayed

## Dependencies

### Build-time Dependencies
- [**Google Test**](https://github.com/google/googletest) (v1.16.0) - Test framework (always fetched via FetchContent)

### Platform Dependencies
- **Windows**: DbgEng (auto-linked by the MSVC STL) for the std backend, kernel32 only for the
  none backend.
- **Linux**: libstdc++ backtrace library (`stdc++exp` or `stdc++_libbacktrace`, found by the
  probe) or `<execinfo.h>` fallback, plus POSIX signal handling.
- **macOS**: `<execinfo.h>` plus POSIX signal handling.

## Key Design Decisions

1. **Event Listener Pattern**: Uses Google Test's event listener interface for non-invasive integration
2. **Stream Redirection**: Temporarily redirects stdout/stderr during test execution to control output
3. **Cross-platform Abstraction**: Platform-specific code isolated in dedicated sections
4. **Header-only Public API**: Simple integration with single include