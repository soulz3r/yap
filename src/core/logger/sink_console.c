#include "logger_internal.h"

#include "cli/colors.h"

#include <stdio.h>

static const char *level_color(yap_log_level level) {
  switch (level) {
    case YAP_LOG_FATAL:
      return YAP_BOLD YAP_RED;
    case YAP_LOG_ERROR:
      return YAP_RED;
    case YAP_LOG_WARN:
      return YAP_ORANGE;
    case YAP_LOG_INFO:
      return YAP_GREEN;
    case YAP_LOG_DEBUG:
      return YAP_BLUE;
    case YAP_LOG_TRACE:
      return YAP_GREY;
    default:
      return YAP_FG;
  }
}

void yap_log_console_write(const char *line, size_t length,
                           yap_log_level level, bool use_colors) {
  if (line == NULL) {
    return;
  }

  if (use_colors) {
    (void)fputs(level_color(level), stderr);
  }

  (void)fwrite(line, sizeof(char), length, stderr);

  if (use_colors) {
    (void)fputs(YAP_RESET, stderr);
  }

  (void)fputc('\n', stderr);
  (void)fflush(stderr);
}
