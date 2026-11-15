# Development guide

This guide describes the current repository workflow and the responsibilities of its main source areas.

## Setup

```sh
git clone https://github.com/soulz3r/yap.git
cd yap
```

Use [Building YAP](building.md) to compile on Linux or Windows.

## Source organization

```text
include/
├── cli/       Public CLI declarations and presentation constants
└── core/      Public reusable core APIs

src/
├── cli/       Dispatch, handlers, help, and platform code
├── core/      Core implementations
└── main.c     Process entry point
```

Public headers are under `include/` and implementations are under `src/`. For example, `include/core/logger/logger.h` declares the logger API implemented under `src/core/logger/`.

Keep private implementation details in source-local headers when possible. Do not expose internal state structures unless callers need them.

## Responsibility boundaries

### Process entry point

`main.c` initializes and shuts down the logger, invokes the CLI, and returns its result. Keep command-specific parsing and behavior out of this file.

### CLI

The CLI owns command names and dispatch, argument parsing, help and usage text, user-facing output, and conversion of core results into terminal messages and exit codes.

It should not become the owner of reusable project-generation logic.

### Core

Core components should accept explicit inputs and return results. They should not depend on how the user supplied those inputs.

For example, a future project-generation API should not need to know whether its configuration came from shell arguments or a TUI form.

### Logger

Use the logger for diagnostics. Do not use log records as a substitute for command output or structured return values.

### Filesystem helpers

Keep low-level filesystem operations small and explicit. Current helpers operate on one directory or file at a time; they do not recursively create missing parent directories.

## C conventions

The project targets C23 and uses integer aliases from `defines.h` in some interfaces.

General expectations:

- Include the header that declares a function.
- Keep public contracts in public headers.
- Validate pointers and sizes at API boundaries.
- Use `size_t` for object sizes and array counts.
- Make ownership and lifetime requirements explicit.
- Check return values from filesystem and formatted-output operations.
- Prefer `const` for read-only input.
- Avoid hidden global state where a caller-owned context is clearer.
- Keep functions focused on one responsibility.

Prefer standard C facilities when sufficient. Isolate platform APIs behind platform definitions.

## Error handling

Core functions should return enough information for the caller to decide what to do.

Examples:

- Filesystem helpers return `bool`.
- The template renderer returns `YAP_TEMPLATE_STATUS`.
- The project executor returns `YAP_PROJECT_RESULT`.

The caller can log diagnostic context and present a useful message. Avoid printing from low-level core functions when a return value can communicate failure.

## User output versus diagnostics

| Situation | Owner |
|---|---|
| Help screen | CLI |
| Command usage error | CLI |
| Progress and success summary | CLI or future TUI |
| Internal filesystem failure detail | Logger |
| Project execution status | Core result |
| Template rendering status | Template API |

A future TUI should present the same core result differently without changing the operation itself.

## Platform-specific code

Platform definitions live in `include/defines.h`. Build scripts explicitly define the platform macro.

Current examples:

- `src/core/utils/make_fd.c` uses `_mkdir()` on Windows and `mkdir()` on Linux.
- `src/cli/cli_linux.c` contains Linux tree traversal.
- `src/cli/cli_windows.c` uses the Win32 file enumeration API for tree traversal and skips reparse-point directories to avoid cycles.
- The Windows tree command temporarily selects the UTF-8 console output code page, flushes its output, and restores the previous code page.

Keep platform-specific terminal handling inside the platform-specific CLI implementation rather than adding it to `main.c`.

Do not claim cross-platform parity merely because a platform macro exists. Build and exercise the feature on the target platform.

## Adding a command

When implementing a command:

1. Define its purpose and argument syntax.
2. Add or update its handler declaration in `include/cli/cli_core.h`.
3. Implement the handler in the CLI layer or a focused command module.
4. Register it in `src/cli/yap_cli.c`.
5. Validate arguments before calling core functionality.
6. Convert core results into user-facing output and a meaningful exit status.
7. Add examples and update [usage.md](usage.md).
8. Verify the behavior on supported platforms.

Do not leave a placeholder handler returning success after doing no work once the command is advertised as implemented.

## Extending project generation

The project-plan executor and template renderer are separate foundations. Before connecting them to a command, define the public project request model and the responsibilities of the generator.

Do not make the CLI own template parsing or filesystem orchestration. Do not make the template renderer responsible for deciding the complete project structure. Keep each layer's inputs and outputs explicit.

See [Project-generation foundations](project-generation.md).

## Validation checklist

Before considering a change complete:

- [ ] The project builds with the relevant platform script.
- [ ] Public functions have declarations and documented contracts.
- [ ] Pointer, size, ownership, and lifetime assumptions are checked.
- [ ] Failure paths return meaningful statuses.
- [ ] Logging does not replace user-facing output.
- [ ] Platform-specific behavior is isolated.
- [ ] Documentation matches actual behavior.
- [ ] Examples compile against the current public API where applicable.

There is not yet a repository-wide automated test suite documented in the current project. Do not report tests as passing unless they have actually been run.
