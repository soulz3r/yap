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
| `new` | Create a project | Placeholder |
| `add` | Add a project component | Placeholder |
| `remove` | Remove a project component | Placeholder |
| `check` | Check project state/configuration | Placeholder |
| `run` | Run a project | Placeholder |
| `clean` | Clean generated build output | Placeholder |
| `tree` | Print a directory tree | Implemented on Linux and Windows |
| `help` | Print help | Implemented |
| `version` | Print version | Implemented |

A command being present in the dispatch table does not mean its feature is ready. Several handlers currently print a placeholder message and return success without performing the intended operation.

## Help

```sh
yap help
yap --help
```

The help screen currently describes the basic invocation and global options. It does not yet document the full dispatch table or command-specific options.

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
- Uses ANSI colors for directory and file names.\n- On Windows, does not recurse into junctions or symbolic-link directories, avoiding directory cycles.

The CLI handler does not currently parse a path argument or filtering options.

### Platform limitation

The Linux implementation is in `src/cli/cli_linux.c`. The Windows implementation in `src/cli/cli_windows.c` is a stub, so equivalent Windows behavior is not available yet.

## New

`new` is intended to become the project-creation command:

```text
yap new ...
```

The current handler is a placeholder. It does not parse project options, select templates, create directories, or write project files.

The project-plan executor and template renderer described in [Project-generation foundations](project-generation.md) are not connected to this command.

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

Because placeholder handlers currently return zero after printing placeholder text, their exit status does not prove that the intended operation occurred.

## Examples

These examples exercise implemented behavior:

```sh
yap --help
yap --version
yap tree
```

The tree example is implemented on Linux and Windows.
