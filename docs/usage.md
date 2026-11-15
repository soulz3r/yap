# CLI usage

This guide documents the command-line interface in the current source tree. It distinguishes working behavior from commands that are registered but unfinished.

## Syntax

```text
yap <command> [arguments]
yap [global-option]
```

The dispatcher reads the first argument as the command name. Global help and version options are checked before command dispatch.

## Global options

| Option | Description |
|---|---|
| `-h`, `--help` | Print help |
| `-v`, `--version` | Print version |

Examples:

```sh
yap --help
yap -h
yap --version
yap -v
```

## Command status

The dispatcher currently registers:

| Command | Intended responsibility | Current status |
|---|---|---|
| `new` | Create a project from a template | Implemented: `default` template |
| `add` | Add a project component | Placeholder |
| `remove` | Remove a project component | Placeholder |
| `check` | Check project state/configuration | Placeholder |
| `run` | Run a project | Placeholder |
| `clean` | Clean generated build output | Placeholder |
| `tree` | Print a directory tree | Implemented on Linux and Windows |
| `help` | Print help | Implemented |
| `version` | Print version | Implemented |

A command being present in the dispatch table does not mean its feature is ready. The `new` command currently supports only the built-in `default` template; the other unfinished handlers remain placeholders.

## Help

```sh
yap help
yap --help
```

The help screen documents the basic invocation, the implemented `new default` syntax, and global options. It does not yet document every registered command.

## Version

```sh
yap version
yap --version
```

The current version function prints the hard-coded development string `Version: 0.0.1`.

## Tree

`tree` prints a directory tree rooted at the current working directory on both Linux and Windows:

```sh
yap tree
```

Current behavior:

- Uses the process working directory as the root.
- Sorts entries alphabetically. Windows sorting is case-insensitive, with a case-sensitive tie-breaker.
- Prints directories with a trailing slash.
- Excludes `.`, `..`, and `.git`.
- Prints total directory and file counts.
- Uses ANSI colors for directory and file names.
- On Windows, temporarily switches the console output code page to UTF-8 while printing the tree, then flushes output and restores the previous code page.
- On Windows, does not recurse into junctions or symbolic-link directories, avoiding directory cycles.

The CLI handler does not currently parse a path argument or filtering options.

### Platform implementation

The Linux implementation is in `src/cli/cli_linux.c`. The Windows implementation in `src/cli/cli_windows.c` uses the Win32 `FindFirstFileA` / `FindNextFileA` APIs to enumerate directory entries. It skips `.`, `..`, and `.git`, sorts names, prints the same tree connectors and totals, and avoids following reparse-point directories such as junctions and symbolic links.

The Windows tree wrapper temporarily calls `SetConsoleOutputCP(CP_UTF8)` when a console output code page is available. It flushes `stdout` before restoring the original code page so buffered tree characters are written using UTF-8. The command still uses the process working directory and does not accept a path argument or filtering options. If Windows cannot read the root directory, it prints an error containing the Windows error code.

## New

The first implemented project-creation path is the built-in `default` template:

```sh
yap new default myproject
```

This creates a new `myproject/` directory in the current working directory:

```text
myproject/
├── include/
│   └── projectname.h
├── src/
│   ├── main.c
│   └── projectname.c
├── build.sh
├── build.bat
└── compile_flags.txt
```

The generated `main.c` includes `projectname.h` and calls `projectname()`. The implementation in `src/projectname.c` prints `Hello from myproject` followed by a newline. The project name is inserted into the generated header as `PROJECT_NAME`. The build scripts compile C sources under `src/` with Clang in C23 mode and write the executable to `bin/<project-name>` (or `bin/<project-name>.exe` on Windows). The compile flags file supplies the C23 standard and the `include/` and `src/` search paths for clangd.

The project name must be 1-64 characters, begin with a letter or number, and contain only letters, numbers, underscores, or hyphens. Windows reserved device names are rejected. The destination directory must not already exist; YAP does not overwrite an existing project.

The Linux script uses Bash, so run it with `bash build.sh` (or make it executable first). The Windows script can be run with `build.bat` or `./build.bat` from PowerShell.

This command uses the existing project-plan executor with a built-in file layout. It does not yet load external templates or expose template-selection options beyond `default`.

## Add, remove, check, run, and clean

These names are registered but their handlers are placeholders:

- `add`
- `remove`
- `check`
- `run`
- `clean`

Their final syntax, flags, exit codes, and side effects have not been defined by complete implementations. This guide intentionally does not invent command-line options for them.

## Missing and unknown commands

Running YAP without a command prints a usage hint and returns a nonzero status:

```sh
yap
```

An unrecognized command is logged as an error and returns a nonzero status:

```sh
yap tre
```

The application suggests using `yap --help` for usage information.

## Exit status

The CLI returns an integer result:

- `0` indicates that the handler returned success.
- A nonzero value indicates that the invocation failed.

The unfinished placeholder handlers currently return zero after printing placeholder text, so their exit status does not prove that the intended operation occurred.

## Examples

These examples exercise implemented behavior:

```sh
yap --help
yap --version
yap new default myproject
yap tree
```

The tree example is implemented on Linux and Windows.
