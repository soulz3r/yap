#pragma once

// What levels of logging are enabled
#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

typedef enum log_level {
  LOG_LEVEL_FATAL = 0, // Anything that makes it not run, like file not found
  LOG_LEVEL_ERROR = 1, // Serious error, but the program can continue
  LOG_LEVEL_WARN = 2,  // Something causing suboptimal behavior
  LOG_LEVEL_INFO = 3,  // Basic information
  LOG_LEVEL_DEBUG = 4, // Debug information, not for release builds
  LOG_LEVEL_TRACE = 5, // Very verbose debugging information
} log_level;

// Two main functions for most systems:
// initialize and shutdown
bool initialize_logging();
void shutdown_logging();

// INFO: Exports the function and acts like printf
void log_output(log_level level, const char *message, ...);

// Fatal level messages
#define YAP_FATAL(message, ...)                                                \
  log_output(LOG_LEVEL_FATAL, message, ##__VA_ARGS__)

// Error level messages
#define YAP_ERROR(message, ...)                                                \
  log_output(LOG_LEVEL_ERROR, message, ##__VA_ARGS__)

// These are checked if enabled or not
#if YAP_WARN_ENABLED == 1

#define YAP_WARN(message, ...)                                                 \
  log_output(LOG_LEVEL_WARN, message, ##__VA_ARGS__)

#else

// Does nothing when LOG_WARN_ENABLED != 1
#define YAP_WARN(message, ...)

#endif

#if LOG_INFO_ENABLED == 1

#define YAP_INFO(message, ...)                                                 \
  log_output(LOG_LEVEL_INFO, message, ##__VA_ARGS__)

#else

// Does nothing when LOG_INFO_ENABLED != 1
#define YAP_INFO(message, ...)

#endif

#if LOG_DEBUG_ENABLED == 1

#define YAP_DEBUG(message, ...)                                                \
  log_output(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__)

#else

// Does nothing when LOG_DEBUG_ENABLED != 1
#define YAP_DEBUG(message, ...)

#endif

#if LOG_TRACE_ENABLED == 1

#define YAP_TRACE(message, ...)                                                \
  log_output(LOG_LEVEL_TRACE, message, ##__VA_ARGS__)

#else

// Does nothing when LOG_TRACE_ENABLED != 1
#define YAP_TRACE(message, ...)

#endif
