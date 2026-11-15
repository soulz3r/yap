#pragma once

#include "defines.h"


/*
 * YAP logging API.
 *
 * Initialize the logger before emitting records. Logging is synchronous:
 * each accepted record is formatted and written before the call returns.
 * The logger is process-wide and is not thread-safe.
 */

typedef enum YAP_LOG_LEVEL {
  YAP_LOG_FATAL = 0,
  YAP_LOG_ERROR,
  YAP_LOG_WARN,
  YAP_LOG_INFO,
  YAP_LOG_DEBUG,
  YAP_LOG_TRACE
} YAP_LOG_LEVEL;

typedef struct YAP_LOG_CONFIG {
  YAP_LOG_LEVEL min_level;

  b console_enabled;
  b file_enabled;

  /* Required when file_enabled is true. The file is opened in append mode. */
  const char *file_path;

  b include_timestamp;
  b include_source;
  b use_colors;
} YAP_LOG_CONFIG;

/* Returns sensible defaults: INFO level, console enabled, file disabled. */
YAP_LOG_CONFIG yap_log_config_default(void);

/*
 * Initializes the process-wide logger.
 * Returns false for invalid configuration or if an enabled sink cannot open.
 * Initialization is transactional: on failure, no sink remains active.
 */
b yap_log_init(const YAP_LOG_CONFIG *config);

/* Flushes and closes owned resources. Safe to call when not initialized. */
void yap_log_shutdown(void);

/* Changes the runtime threshold. Returns false if the logger is not initialized
 * or level is outside the YAP_LOG_LEVEL range.
 */
b yap_log_set_level(YAP_LOG_LEVEL level);

/* Returns whether a record at this level would pass the current filter. */
b yap_log_is_enabled(YAP_LOG_LEVEL level);

/*
 * Implementation entry point used by the public macros.
 * Prefer YAP_LOG_* at call sites so source location is captured automatically.
 */
void yap_log_write(YAP_LOG_LEVEL level, const char *module, const char *file,
                   int line, const char *function, const char *format, ...);

/*
 * The format string is the first variadic argument, so calls with or without
 * additional format arguments are valid standard C variadic-macro usage.
 */
#define YAP_LOG_FATAL(module, ...)                                             \
  yap_log_write(YAP_LOG_FATAL, (module), __FILE__, __LINE__, __func__,         \
                __VA_ARGS__)

#define YAP_LOG_ERROR(module, ...)                                             \
  yap_log_write(YAP_LOG_ERROR, (module), __FILE__, __LINE__, __func__,         \
                __VA_ARGS__)

#define YAP_LOG_WARN(module, ...)                                              \
  yap_log_write(YAP_LOG_WARN, (module), __FILE__, __LINE__, __func__,          \
                __VA_ARGS__)

#define YAP_LOG_INFO(module, ...)                                              \
  yap_log_write(YAP_LOG_INFO, (module), __FILE__, __LINE__, __func__,          \
                __VA_ARGS__)

#define YAP_LOG_DEBUG(module, ...)                                             \
  yap_log_write(YAP_LOG_DEBUG, (module), __FILE__, __LINE__, __func__,         \
                __VA_ARGS__)

#define YAP_LOG_TRACE(module, ...)                                             \
  yap_log_write(YAP_LOG_TRACE, (module), __FILE__, __LINE__, __func__,         \
                __VA_ARGS__)
