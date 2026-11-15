# Troubleshooting

This guide covers common problems with the current build workflow and source implementation.

## Build problems

### Clang cannot be found

The build scripts invoke `clang`. Verify it is installed and available on `PATH`:

```sh
clang --version
```

On Windows, run this command in the same terminal used to build YAP. If it is not recognized, add the LLVM binary directory to `PATH`.

### Clang rejects `-std=c23`

The installed compiler may be too old. Install a Clang release with C23 support. The project explicitly requests C23; changing the language standard may hide compatibility problems.

### Source files are not found

Run the build script from the repository root. It searches beneath `src/` and uses relative include paths.

### Windows build fails before compilation

The Windows script expects 64-bit Windows and a Clang executable named `clang`. Verify the compiler and environment first.

### A source file is not compiled

The scripts discover files ending in `.c` recursively beneath `src/`. Check the extension and location.

### Build succeeds but a command is unfinished

A successful compile means the current source compiled. It does not mean every registered command is implemented. Check [Command status](usage.md#command-status).

## Runtime problems

### Unknown command

Command names must match a registered name. Run:

```sh
yap --help
```

The dispatcher currently registers `new`, `add`, `remove`, `check`, `run`, `clean`, `tree`, `help`, and `version`.

### A command prints a placeholder message

Several handlers are unfinished. The current `new`, `add`, `remove`, `check`, `run`, and `clean` handlers do not perform their intended operations. See [CLI usage](usage.md#command-status).

### Tree cannot read a directory on Windows

The Windows implementation uses Win32 file enumeration. Check that the current working directory exists and that the process has permission to list it. YAP prints the Windows error code when it cannot open the root or enumerate a directory.

The command does not follow junctions or symbolic-link directories. Those entries are displayed, but their targets are not traversed, which prevents directory cycles.

### Tree connectors display as garbled characters on Windows

The Windows tree command temporarily switches the console output code page to UTF-8 while printing and restores the previous code page after flushing `stdout`.

If the connectors still appear corrupted, check the terminal's output encoding and confirm that the terminal supports UTF-8. In PowerShell, inspect the current settings with:

```powershell
[Console]::OutputEncoding
chcp
```

## Logger problems

### No log messages appear

The logger must be initialized first. The default threshold is INFO, so DEBUG and TRACE records are filtered.

Check that at least one sink is enabled and the configured threshold permits the message level.

### Logger initialization fails

Check that:

- the config pointer is not null;
- the log level is valid;
- at least one sink is enabled;
- file path is non-null and non-empty when file output is enabled;
- the process can open the requested file.

### File logging stops

When a file write fails, the implementation closes and disables the file sink. Console logging can continue if enabled. Check the path, permissions, and available disk space.

## Project-plan problems

### Generation returns an invalid-plan status

Check that:

- the plan pointer is valid;
- the root path is non-empty;
- the operations pointer is non-null when count is nonzero;
- every operation path is a non-empty relative path;
- paths do not contain empty segments, `.`, `..`, or colon characters;
- file-writing operations have non-null content;
- each operation type is recognized.

### A directory or file operation fails

The root must not already exist. Parent directories must be created before files inside them. The current filesystem helpers do not recursively create missing parent directories.

### Some files exist even though generation failed

The executor does not roll back completed operations. It reports completed progress and the failed operation index. Remove partial output manually only after verifying the target directory.

## Template-rendering problems

### Unknown variable

Every expression must match a variable name in the supplied context. Names are case-sensitive. Whitespace around an expression name is ignored.

For example, `{{project.name}}` requires a context entry named exactly `project.name`.

### Unterminated expression

Every opening `{{` must have a matching `}}`.

### Output buffer too small

The renderer writes to a caller-owned buffer. Increase its capacity or calculate a suitable size before rendering. The renderer does not allocate a larger buffer automatically.

### Renderer does not create files

That is expected. The template renderer only produces text. The caller must use the rendered content when constructing a project plan.

## Reporting a bug

Include:

- Operating system and architecture;
- Clang version;
- exact build command;
- complete first compiler diagnostic or runtime output;
- command and arguments used;
- whether the issue reproduces from a clean clone.

A precise reproduction is much easier to debug than “it doesn't work”.
