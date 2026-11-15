# Architecture

This document describes the current source organization and the boundaries the code is beginning to establish. Planned components are explicitly identified as planned.

## Design direction

YAP is intended to separate the interface from the work being performed.

The CLI should parse arguments and communicate the user's request. Reusable core components should perform the operation without depending on the CLI. A future TUI should collect the same request and call the same core functionality.

```text
              CLI
               |
               v
       Request / configuration
               |
               v
          Core logic
               |
               v
       Filesystem operations
               |
               v
         Structured result
               |
               v
       CLI or future TUI output
```

This is the intended direction, not the current end-to-end implementation. At present, most command handlers are placeholders and the project-plan executor and template renderer are not wired into `yap new`.

## Process entry point

`src/main.c` is the process entry point. It:

1. Creates the default logger configuration.
2. Initializes the logger.
3. Calls `yap_cli(argc, argv)`.
4. Prints a usage hint if the CLI returns nonzero.
5. Shuts down the logger.
6. Returns the CLI result as the process exit status.

Keep `main()` small. Command parsing belongs in the CLI layer.

## CLI layer

| File | Responsibility |
|---|---|
| `include/cli/yap_cli.h` | CLI entry-point declaration and command structure |
| `src/cli/yap_cli.c` | Global options and command dispatch |
| `include/cli/cli_core.h` | Command handler declarations |
| `src/cli/cli_core.c` | Current command handlers |
| `src/cli/help.c` | Help and version output |
| `src/cli/cli_linux.c` | Linux directory-tree implementation |
| `src/cli/cli_windows.c` | Windows directory-tree implementation using Win32 file enumeration and UTF-8 console output handling |
| `include/cli/colors.h` | ANSI color and style constants |

The dispatch table maps a command string to a function pointer. Each handler receives the command name and remaining arguments through `argc` and `argv`.

The CLI owns command parsing and user-facing terminal output. Reusable core APIs should not print CLI-specific messages.

## Core layer

### Filesystem helpers

Public header: `include/core/utils/make_fd.h`.

```c
bool create_dir(const char *dir);
bool create_file(const char *path);
bool write_file(const char *path, const char *content);
```

- `create_dir()` creates one directory.
- `create_file()` creates a file if it does not exist, without truncating an existing file.
- `write_file()` writes text content, replacing existing content.

These helpers do not recursively create missing parent directories.

### Project-plan executor

The public API is in `include/core/project/project.h`. A `YAP_PROJECT_PLAN` contains a root directory and an ordered array of operations. Each operation creates a directory or writes a file.

`yap_project_generate()` validates the plan, creates the root, joins each relative path to the root, and executes operations in array order.

Important contract details:

- The root directory must not already exist.
- Operation paths are relative to the root.
- Parent directories must be created before files that use them.
- The caller retains ownership of the plan and all referenced strings.
- A failed operation can leave earlier operations on disk; there is no rollback.

The executor runs a supplied plan. It does not decide what a project should contain.

### Template renderer

The public API is in `include/core/template/template.h`. It substitutes expressions such as `{{project.name}}` using a caller-provided variable context.

The renderer uses a caller-owned output buffer and does not allocate one itself.

Current support is limited to variable substitution. There is no template package loader, conditional syntax, loop syntax, include system, or project configuration parser.

### Logger

The logger is a process-wide diagnostic facility. It formats records and writes to enabled sinks. See [Logger](logger.md).

## Ownership

The current project and template APIs use caller-owned data:

- The caller owns the project plan and its strings.
- The caller owns the template context and its variable strings.
- The caller owns the template output buffer.
- Neither API takes ownership of those objects.

Keep ownership and lifetime requirements explicit when extending these APIs.

## Platform boundary

Platform macros are defined in `include/defines.h`. The supplied build scripts define either `YAP_PLATFORM_WINDOWS` or `YAP_PLATFORM_LINUX`.

Examples:

- `src/core/utils/make_fd.c` uses `_mkdir()` on Windows and `mkdir()` on Linux.
- `src/cli/cli_linux.c` contains Linux directory traversal.
- `src/cli/cli_windows.c` uses `FindFirstFileA()` and `FindNextFileA()` for Windows directory traversal. It does not recurse into reparse-point directories, preventing junction and symbolic-link cycles. The tree output temporarily switches the console output code page to UTF-8 and restores the previous code page after flushing its output.

Fallback macros in `defines.h` do not mean every Unix-like platform is supported.

## Current gaps

The intended complete workflow still needs:

1. A stable project request/configuration type.
2. CLI argument parsing that fills that configuration.
3. A project-generation coordinator that validates configuration and selects a template.
4. A template definition format and loader.
5. A way to turn rendered templates into project-plan operations.
6. Integration of the plan executor with the template renderer.
7. Structured results for the CLI/TUI to present.
8. A TUI that uses the same core workflow.

These are future implementation tasks, not current features.
