#include "core/logger/logger.h"

#include "defines.h"
#include "logger_internal.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

YAP_LOG_STATE yap_logger = {0};

static FILE *yap_log_open_file(const char *path, const char *mode) {
#if YAP_PLATFORM_WINDOWS
  FILE *file = NULL;
  if (fopen_s(&file, path, mode) != 0) {
    return NULL;
  }
  return file;
#else
  return fopen(path, mode);
#endif
}

static b yap_log_level_valid(YAP_LOG_LEVEL level) {
  return level >= YAP_LOG_FATAL && level <= YAP_LOG_TRACE;
}

const char *yap_log_level_name(YAP_LOG_LEVEL level) {
  switch (level) {
    case YAP_LOG_FATAL: return "FATAL";
    case YAP_LOG_ERROR: return "ERROR";
    case YAP_LOG_WARN: return "WARN";
    case YAP_LOG_INFO: return "INFO";
    case YAP_LOG_DEBUG: return "DEBUG";
    case YAP_LOG_TRACE: return "TRACE";
    default: return "UNKNOWN";
  }
}

YAP_LOG_CONFIG yap_log_config_default(void) {
  return (YAP_LOG_CONFIG){
      .min_level = YAP_LOG_INFO,
      .console_enabled = true,
      .file_enabled = false,
      .file_path = NULL,
      .include_timestamp = true,
      .include_source = false,
      .use_colors = true,
  };
}

b yap_log_init(const YAP_LOG_CONFIG *config) {
  if (config == NULL || !yap_log_level_valid(config->min_level)) {
    return false;
  }

  if (!config->console_enabled && !config->file_enabled) {
    return false;
  }

  if (config->file_enabled &&
      (config->file_path == NULL || config->file_path[0] == '\0')) {
    return false;
  }

  FILE *file = NULL;
  if (config->file_enabled) {
    file = yap_log_open_file(config->file_path, "a");
    if (file == NULL) {
      return false;
    }
  }

  yap_log_shutdown();

  yap_logger = (YAP_LOG_STATE){
      .initialized = true,
      .min_level = config->min_level,
      .console_enabled = config->console_enabled,
      .file_enabled = config->file_enabled,
      .include_timestamp = config->include_timestamp,
      .include_source = config->include_source,
      .use_colors = config->use_colors && config->console_enabled,
      .file = file,
  };

  return true;
}

void yap_log_shutdown(void) {
  if (yap_logger.file != NULL) {
    (void)fflush(yap_logger.file);
    (void)fclose(yap_logger.file);
  }

  yap_logger = (YAP_LOG_STATE){0};
}

b yap_log_set_level(YAP_LOG_LEVEL level) {
  if (!yap_logger.initialized || !yap_log_level_valid(level)) {
    return false;
  }

  yap_logger.min_level = level;
  return true;
}

b yap_log_is_enabled(YAP_LOG_LEVEL level) {
  return yap_logger.initialized && yap_log_level_valid(level) &&
         level <= yap_logger.min_level;
}

void yap_log_write(YAP_LOG_LEVEL level, const char *module, const char *file,
                   i32 line, const char *function, const char *format, ...) {
  if (!yap_log_is_enabled(level) || format == NULL) {
    return;
  }

  YAP_LOG_RECORD record = {
      .level = level,
      .module = (module != NULL && module[0] != '\0') ? module : "APP",
      .file = file,
      .function = function,
      .line = line,
  };

  if (yap_logger.include_timestamp &&
      !yap_log_timestamp(record.timestamp, sizeof(record.timestamp))) {
    (void)snprintf(record.timestamp, sizeof(record.timestamp), "%s",
                   "unknown-time");
  }

  va_list args;
  va_start(args, format);
  const i32 message_length =
      vsnprintf(record.message, sizeof(record.message), format, args);
  va_end(args);

  if (message_length < 0) {
    (void)snprintf(record.message, sizeof(record.message), "%s",
                   "log message formatting failed");
  } else if ((st)message_length >= sizeof(record.message)) {
    static const char marker[] = "...[truncated]";
    const st marker_size = sizeof(marker);
    const st offset = sizeof(record.message) - marker_size;
    memcpy(record.message + offset, marker, marker_size);
  }

  char line_buffer[YAP_LOG_LINE_CAPACITY];
  st line_length = 0;

  if (!yap_log_format_record(&record, yap_logger.include_timestamp,
                             yap_logger.include_source, line_buffer,
                             sizeof(line_buffer), &line_length)) {
    return;
  }

  if (yap_logger.console_enabled) {
    yap_log_console_write(line_buffer, line_length, record.level,
                          yap_logger.use_colors);
  }

  if (yap_logger.file_enabled && yap_logger.file != NULL) {
    yap_log_file_write(yap_logger.file, line_buffer, line_length);

    if (ferror(yap_logger.file)) {
      (void)fclose(yap_logger.file);
      yap_logger.file = NULL;
      yap_logger.file_enabled = false;
    }
  }
}
