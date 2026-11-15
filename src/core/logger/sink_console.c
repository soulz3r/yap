#include "logger_internal.h"

#include <stdio.h>

static const char *level_color(yap_log_level level) {
    switch (level) {
        case YAP_LOG_FATAL: return "\x1b[1;31m";
        case YAP_LOG_ERROR: return "\x1b[31m";
        case YAP_LOG_WARN:  return "\x1b[33m";
        case YAP_LOG_INFO:  return "\x1b[32m";
        case YAP_LOG_DEBUG: return "\x1b[36m";
        case YAP_LOG_TRACE: return "\x1b[90m";
        default:            return "\x1b[0m";
    }
}

void yap_log_console_write(
    const char *line,
    size_t length,
    yap_log_level level,
    bool use_colors
) {
    if (line == NULL) {
        return;
    }

    if (use_colors) {
        (void)fputs(level_color(level), stderr);
    }

    (void)fwrite(line, sizeof(char), length, stderr);

    if (use_colors) {
        (void)fputs("\x1b[0m", stderr);
    }

    (void)fputc('\n', stderr);
    (void)fflush(stderr);
}
