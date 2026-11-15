# YAP

**Yet Another Project** is a small cross-platform project utility written in C.

YAP is intended to make common project setup and maintenance tasks predictable and repeatable. The long-term design separates the interface from the work: the CLI collects a request, reusable core components perform the operation, and a future TUI can use the same core without duplicating project logic.

> **Development status:** YAP is under active development. Several registered commands are placeholders. The project-plan executor and template-variable renderer exist as low-level foundations, but they are not yet connected to `yap new` as a complete project-generation workflow. See [Current status](#current-status).

## Contents

- [Overview](#overview)
- [Current status](#current-status)
- [Requirements](#requirements)
- [Build](#build)
- [Run](#run)
- [Commands](#commands)
- [Project layout](#project-layout)
- [Core components](#core-components)
- [Platform support](#platform-support)
- [Documentation](#documentation)
- [Development](#development)
- [License](#license)

## Overview

The intended workflow is to describe a project or action, validate that request, resolve the required structure, prepare filesystem operations, execute them, and report a useful result.

The CLI is one interface to this workflow. A future TUI should be another interface to the same functionality. The core should not need to know whether a request came from command-line arguments or an interactive terminal interface.

YAP is written in C and currently targets C23. Clang is used by the supplied build scripts.

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
.\build.bat
```

The script discovers C files recursively under `src` and creates `bin\yap.exe`.

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
.\build.bat
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
.\bin\yap.exe --help
.\bin\yap.exe --version
.\bin\yap.exe tree
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
- [Troubleshooting](docs/troubleshooting.md)

## Development

YAP is written in C23. Keep platform-specific behavior explicit, validate public API inputs, make ownership clear, and separate user-facing output from diagnostic logging.

When implementing a feature, document only behavior that exists and update the relevant guide when behavior changes. See [Development guide](docs/development.md).

## License

YAP is distributed under the [MIT License](LICENSE).
