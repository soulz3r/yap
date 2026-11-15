#include "logger_internal.h"

#include "defines.h"

#include "cli/colors.h"

#include <stdio.h>

static const char *level_color(YAP_LOG_LEVEL level) {
  switch (level) {
    case YAP_LOG_FATAL:
      return YAP_BOLD YAP_RED;
    case YAP_LOG_ERROR:
      return YAP_RED;
    case YAP_LOG_WARN:
      return YAP_YELLOW;
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

void yap_log_console_write(const char *line, st length,
                           YAP_LOG_LEVEL level, b use_colors) {
  if (line == NULL) {
    return;
  }

  if (!use_colors) {
    (void)fwrite(line, sizeof(char), length, stderr);
    (void)fflush(stderr);
    return;
  }

  /* Color only the severity label, not the entire log message. */
  st level_length = 0;
  while (level_length < length && line[level_length] != ']') {
    level_length++;
  }

  if (level_length == length) {
    (void)fwrite(line, sizeof(char), length, stderr);
    (void)fflush(stderr);
    return;
  }

  level_length++;

  (void)fputs(level_color(level), stderr);
  (void)fwrite(line, sizeof(char), level_length, stderr);
  (void)fputs(YAP_RESET, stderr);

  st offset = level_length;

  /* The timestamp is the next bracketed field. Keep it muted grey. */
  if (offset + 1 < length && line[offset] == ' ' &&
      line[offset + 1] == '[') {
    st timestamp_end = offset + 2;

    while (timestamp_end < length && line[timestamp_end] != ']') {
      timestamp_end++;
    }

    if (timestamp_end < length) {
      timestamp_end++;

      (void)fwrite(line + offset, sizeof(char), 1, stderr);
      (void)fputs(YAP_GREY, stderr);
      (void)fwrite(line + offset + 1, sizeof(char),
                   timestamp_end - offset - 1, stderr);
      (void)fputs(YAP_RESET, stderr);

      offset = timestamp_end;
    }
  }

  /* Message and optional source location use the terminal's normal color. */
  (void)fwrite(line + offset, sizeof(char), length - offset, stderr);
  (void)fflush(stderr);
}
