# Logger

YAP's logger is a diagnostic facility for messages about internal behavior, failures, and useful debugging information.

It is not the normal output interface for commands. Help text, progress messages, success summaries, and other user-facing output belong to the CLI or a future TUI. The logger keeps diagnostic records separate from that output.

## Public header

Include:

```c
#include "core/logger/logger.h"
```

The public API defines `YAP_LOG_LEVEL`, `YAP_LOG_CONFIG`, initialization/shutdown functions, runtime level control, a write function, and macros from `YAP_LOG_FATAL` through `YAP_LOG_TRACE`.

## Basic usage

Initialize the logger before writing records:

```c
#include "core/logger/logger.h"

int main(void) {
  YAP_LOG_CONFIG config = yap_log_config_default();

  if (!yap_log_init(&config)) {
    return 1;
  }

  YAP_LOG_INFO("APP", "Application started");

  yap_log_shutdown();
  return 0;
}
```

The default configuration enables console output at INFO level and disables file output.

Call `yap_log_shutdown()` before normal process exit to flush and release resources.

## Log levels

From most severe to most verbose:

| Level | Intended use |
|---|---|
| `YAP_LOG_FATAL` | Critical failure; the program cannot safely continue |
| `YAP_LOG_ERROR` | An operation failed |
| `YAP_LOG_WARN` | Unexpected or incomplete condition, but execution can continue |
| `YAP_LOG_INFO` | Useful high-level diagnostic information |
| `YAP_LOG_DEBUG` | Details useful during debugging |
| `YAP_LOG_TRACE` | Very detailed execution information |

`min_level` is a severity threshold. A record is enabled when its level is at least as severe as the configured threshold. With INFO selected, FATAL, ERROR, WARN, and INFO pass; DEBUG and TRACE are filtered.

```c
if (yap_log_is_enabled(YAP_LOG_DEBUG)) {
  YAP_LOG_DEBUG("PROJECT", "Project root: %s", root);
}
```

Change the threshold after initialization with:

```c
if (!yap_log_set_level(YAP_LOG_DEBUG)) {
  YAP_LOG_WARN("LOGGER", "Could not change log level");
}
```

## Logging macros

Prefer the macros at call sites:

```c
YAP_LOG_INFO("CLI", "Command received: %s", command);
YAP_LOG_WARN("PROJECT", "Optional configuration was not found");
YAP_LOG_ERROR("FILESYSTEM", "Could not create directory: %s", path);
YAP_LOG_DEBUG("TEMPLATE", "Rendering variable: %s", name);
YAP_LOG_TRACE("TEMPLATE", "Read %zu bytes", length);
```

Each macro supplies the severity, module, source file, line, function, and format arguments to `yap_log_write()`.

Use a short module label such as `CLI`, `FILESYSTEM`, `PROJECT`, or `TEMPLATE`. Format strings follow printf conventions. Use `%zu` for `size_t` and `%s` for strings.

## Configuration

`yap_log_config_default()` returns:

| Field | Default | Meaning |
|---|---:|---|
| `min_level` | `YAP_LOG_INFO` | Minimum severity accepted |
| `console_enabled` | `true` | Write to console |
| `file_enabled` | `false` | Write to file |
| `file_path` | `NULL` | Required when file output is enabled |
| `include_timestamp` | `true` | Include timestamp |
| `include_source` | `false` | Include source information |
| `use_colors` | `true` | Colorize console records |

At least one sink must be enabled. When file output is enabled, `file_path` must be non-null and non-empty. The file is opened in append mode.

Example:

```c
YAP_LOG_CONFIG config = yap_log_config_default();

config.min_level = YAP_LOG_DEBUG;
config.console_enabled = true;
config.file_enabled = true;
config.file_path = "yap.log";
config.include_timestamp = true;
config.include_source = true;
config.use_colors = true;

if (!yap_log_init(&config)) {
  return 1;
}
```

## Record output

A representative record is:

```text
[WARN] [14:25:02.454] [FILESYSTEM] Failed to create directory: build
```

The exact fields depend on formatter settings. Console output may contain ANSI colors. File output is written without console color decoration.

## Internal flow

A logging macro calls `yap_log_write()`. The write function:

1. Checks initialization and the severity threshold.
2. Builds a record and selects a default module label if necessary.
3. Obtains a timestamp when enabled.
4. Formats the variadic message into a fixed-size message buffer.
5. Marks messages that exceed the buffer as truncated.
6. Formats the record into a line buffer.
7. Writes to the console sink when enabled.
8. Writes to the file sink when enabled.

The logger uses fixed-size buffers rather than allocating a new message buffer for every record.

## Failure behavior

- Invalid configuration causes initialization to fail.
- Failure to open an enabled file sink causes initialization to fail.
- A formatting failure produces a fallback diagnostic message.
- Oversized messages are truncated and marked.
- A file write error closes and disables the file sink.

The write function returns `void`. Do not use logging as the result channel for an operation. Return operation status through that operation's API.

## Lifecycle and constraints

- Initialize before emitting records.
- The logger is process-wide.
- Logging is synchronous.
- The current implementation is not thread-safe.
- Shutdown flushes and releases owned resources.
- Shutdown is safe when the logger is not initialized.

The logger is not designed for use inside signal handlers; standard I/O and formatting are not async-signal-safe.

## Logger versus user-facing output

| Situation | Correct mechanism |
|---|---|
| Help screen | CLI output |
| “Project created successfully” | CLI/TUI output |
| Unknown command diagnostic | Logger, plus a usage hint |
| Failed directory creation | Logger and returned failure status |
| Missing template variable | Template status; caller decides presentation |
| Internal debugging detail | Logger |

The logger explains internal diagnostics. The interface tells the user what happened and what they can do next.
