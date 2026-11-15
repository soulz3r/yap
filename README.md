# YAP

**Yet Another Project** is an early-stage C project utility under active development.

**Current reality:** YAP is not yet a usable project generator. The `new`, `add`, `remove`, `check`, `run`, and `clean` commands are placeholders. The working parts are the CLI dispatcher, help/version output, Linux tree traversal, logger, basic filesystem helpers, and two separate low-level APIs for executing a project plan and rendering template variables.

The documentation covers both the code that exists and the architecture being considered. Sections describing a future generator or TUI are design notes, not claims that those features have been implemented. Start with [Current status](#current-status) before treating any command or subsystem as available.

## Overview

YAP is being developed as a C-based project setup and maintenance tool. The current implementation is a collection of foundations rather than a complete workflow.

The intended direction is to keep interfaces separate from the work: a CLI, and eventually a TUI, should collect a request and pass it to reusable core functionality. That end-to-end workflow does not exist yet.

YAP currently targets C23. The supplied build scripts use Clang.

## Current status

The following table distinguishes implemented behavior from registered command names and unfinished foundations.

| Area | Current state |
|---|---|
| CLI dispatch | Implemented |
| `help` / `--help` | Implemented |
| `version` / `--version` | Implemented |
| `tree` | Linux implementation exists; Windows implementation is a placeholder |
| `new` | Placeholder; does not generate a project |
| `add` | Placeholder |
| `remove` | Placeholder |
| `check` | Placeholder |
| `run` | Placeholder |
| `clean` | Placeholder |
| Logger | Implemented diagnostic component |
| Filesystem helpers | Basic directory creation, file creation, and file writing |
| Project-plan executor | Implemented low-level API; not wired into the CLI |
| Template renderer | Implements `{{variable.name}}` substitution; not wired into project generation |
| Complete project-generation workflow | Not yet implemented |
| TUI | Planned, not implemented |

A command being present in the dispatcher does not mean the feature is ready. In particular, the current `yap new` handler is a placeholder and does not create a project.

## Requirements

### Build requirements

- Clang with C23 support.
- Bash and standard POSIX tools for the Linux build.
- 64-bit Windows and Clang for the Windows build.

The scripts compile C files under `src/` and search headers under `include/` and `src/`.

### Runtime requirements

YAP currently has no third-party runtime library dependency.

## Build

Clone the repository and enter it:

```sh
git clone https://github.com/soulz3r/yap.git
cd yap
```

### Linux

```sh
chmod +x build.sh
./build.sh
```

The script discovers all `.c` files under `src/`, creates `bin/`, and builds `bin/yap` with Clang in C23 mode.

### Windows

Run from PowerShell or Command Prompt:

```powershell
.\\build.bat
```

The script discovers C files recursively under `src` and creates `bin\\yap.exe`.

### Compiler flags

Both scripts use `-std=c23 -g -O0 -Wall` and include paths `-Iinclude -Isrc`. The platform script defines either `YAP_PLATFORM_LINUX` or `YAP_PLATFORM_WINDOWS`.

An optional `compilerFlags` environment variable is appended to the compiler invocation.

Bash example:

```sh
compilerFlags="-Wextra -Wpedantic" ./build.sh
```

PowerShell example:

```powershell
$env:compilerFlags = "-Wextra -Wpedantic"
.\\build.bat
Remove-Item Env:compilerFlags
```

See [Building YAP](docs/building.md) for details.

## Run

Linux:

```sh
./bin/yap --help
./bin/yap --version
./bin/yap tree
```

Windows:

```powershell
.\\bin\\yap.exe --help
.\\bin\\yap.exe --version
.\\bin\\yap.exe tree
```

YAP expects a command as the first argument. Running it without a command prints a usage hint and returns a nonzero status.

## Commands

| Command | Intended purpose | Current behavior |
|---|---|---|
| `new` | Create a project | Placeholder |
| `add` | Add a project component | Placeholder |
| `remove` | Remove a project component | Placeholder |
| `check` | Check project state | Placeholder |
| `run` | Run a project | Placeholder |
| `clean` | Clean build artifacts | Placeholder |
| `tree` | Display a directory tree | Linux implementation; Windows unfinished |
| `help` | Display help | Implemented |
| `version` | Display version | Implemented |

Global options:

| Option | Purpose |
|---|---|
| `-h`, `--help` | Display help |
| `-v`, `--version` | Display version |

Command-specific arguments are not stable yet. See [CLI usage](docs/usage.md) for the exact current behavior and limitations.

## Project layout

```text
yap/
├── build.bat
├── build.sh
├── compile_flags.txt
├── include/
│   ├── cli/
│   └── core/
│       ├── logger/
│       ├── project/
│       ├── template/
│       └── utils/
├── src/
│   ├── cli/
│   ├── core/
│   │   ├── logger/
│   │   ├── project/
│   │   ├── template/
│   │   └── utils/
│   └── main.c
└── README.md
```

This describes the current source organization, not a guarantee that every subsystem is complete.

## Core components

- **CLI:** global option handling and command dispatch.
- **Logger:** diagnostic records written to enabled sinks.
- **Filesystem helpers:** basic single-directory and file operations.
- **Project-plan executor:** executes an ordered caller-provided list of directory/file operations.
- **Template renderer:** substitutes named variables in caller-provided text.

The project executor and template renderer are currently separate. The caller must coordinate them; no template package loader or complete project-generation coordinator exists yet.

## Platform support

| Platform | Build script | Notes |
|---|---|---|
| Linux | `build.sh` | Clang C23 build; Linux tree traversal exists |
| Windows 64-bit | `build.bat` | Clang C23 build; tree implementation is unfinished |
| Other Unix-like systems | None provided | Not currently a documented target |

Do not assume full platform parity simply because both build scripts exist.

## Documentation

- [Building YAP](docs/building.md)
- [CLI usage and command reference](docs/usage.md)
- [Architecture and source organization](docs/architecture.md)
- [Logger API and examples](docs/logger.md)
- [Project-generation foundations](docs/project-generation.md)
- [Development guide](docs/development.md)
- [Design decisions](docs/design-decisions.md)
- [Contributing and maintenance](docs/contributing.md)
- [Troubleshooting](docs/troubleshooting.md)

## Development

YAP is written in C23. Keep platform-specific behavior explicit, validate public API inputs, make ownership clear, and separate user-facing output from diagnostic logging.

When implementing a feature, document only behavior that exists and update the relevant guide when behavior changes. See [Development guide](docs/development.md).

## License

YAP is distributed under the [MIT License](LICENSE).
