# testcoe Architecture

## Overview

testcoe enhances Google Test by intercepting test events and providing visual feedback. It consists of several modular components working together.

## Component Architecture

```
┌─────────────────────────────────────┐
│         Test Application            │
│        (Your Google Tests)          │
└───────────────┬─────────────────────┘
                │
┌───────────────▼─────────────────────┐
│            testcoe                  │
│    (Main Interface & API)           │
└───────────────┬─────────────────────┘
                │
        ┌───────┴────────┬─────────────┐
        │                │             │
┌───────▼────────┐ ┌─────▼──────┐ ┌───▼──────────┐
│ GridListener   │ │SignalHandler│ │TerminalUtils│
│ (Visual Grid)  │ │  (Crashes)  │ │  (Terminal)  │
└────────────────┘ └────────────┘ └──────────────┘
```

## Core Components

### testcoe (Main Interface)
- **File**: `src/testcoe.cpp`, `include/testcoe/testcoe.hpp`
- **Purpose**: Provides the public API for initialization and test execution
- **Key Functions**:
  - `init()` - Initializes Google Test and installs custom listeners
  - `run()` - Executes all tests
  - `run_suite()` - Executes specific test suite
  - `run_test()` - Executes specific test

### GridListener
- **File**: `src/grid_listener.cpp`, `include/testcoe/grid_listener.hpp`
- **Purpose**: Implements Google Test event listener for visual grid display
- **Key Features**:
  - Tracks test execution state
  - Updates terminal display in real-time
  - Collects and displays failure information
  - Shows execution time statistics

### SignalHandler
- **File**: `src/signal_handler.cpp`, `include/testcoe/signal_handler.hpp`
- **Purpose**: Catches crashes and provides detailed stack traces
- **Platform Support**:
  - **Unix/Linux/macOS**: POSIX signal handlers (SIGSEGV, SIGABRT, etc.)
  - **Windows**: Structured Exception Handling (SEH)
- **Uses backward-cpp for stack trace generation**

### TerminalUtils
- **File**: `src/terminal_utils.cpp`, `include/testcoe/terminal_utils.hpp`
- **Purpose**: Cross-platform terminal operations
- **Key Functions**:
  - `isAnsiEnabled()` - Detects ANSI color support
  - `clear()` - Clears terminal screen

## Data Flow

1. **Initialization**:
   - User calls `testcoe::init()`
   - Google Test is initialized
   - GridListener is installed
   - Signal handlers are registered

2. **Test Execution**:
   - User calls `testcoe::run()`
   - Google Test begins execution
   - GridListener receives events:
     - `OnTestProgramStart` - Initialize grid display
     - `OnTestStart` - Mark test as running
     - `OnTestEnd` - Mark test as passed/failed
     - `OnTestProgramEnd` - Show final summary

3. **Crash Handling**:
   - If a test crashes, signal handler is triggered
   - Stack trace is generated using backward-cpp
   - Output streams are restored
   - Detailed crash report is displayed

## Dependencies

### Build-time Dependencies
- [**Google Test**](https://github.com/google/googletest) (v1.16.0) - Test framework
- [**backward-cpp**](https://github.com/bombela/backward-cpp) (v1.6) - Stack trace generation

### Platform Dependencies
- **Windows**: DbgHelp.dll (for symbol resolution)
- **Unix/Linux**: Standard POSIX signal handling
- **macOS**: Standard POSIX signal handling

## Key Design Decisions

1. **Event Listener Pattern**: Uses Google Test's event listener interface for non-invasive integration
2. **Stream Redirection**: Temporarily redirects stdout/stderr during test execution to control output
3. **Cross-platform Abstraction**: Platform-specific code isolated in dedicated sections
4. **Header-only Public API**: Simple integration with single include