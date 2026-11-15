#pragma once

#include "core/logger/logger.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#define YAP_LOG_MESSAGE_CAPACITY 2048
#define YAP_LOG_LINE_CAPACITY 4096
#define YAP_LOG_TIMESTAMP_CAPACITY 32

typedef struct yap_log_record {
    yap_log_level level;
    const char *module;
    const char *file;
    const char *function;
    int line;

    char timestamp[YAP_LOG_TIMESTAMP_CAPACITY];
    char message[YAP_LOG_MESSAGE_CAPACITY];
} yap_log_record;

typedef struct yap_logger_state {
    bool initialized;
    yap_log_level min_level;

    bool console_enabled;
    bool file_enabled;
    bool include_timestamp;
    bool include_source;
    bool use_colors;

    FILE *file;
} yap_logger_state;

extern yap_logger_state yap_logger;

const char *yap_log_level_name(yap_log_level level);
bool yap_log_timestamp(char *buffer, size_t capacity);
bool yap_log_format_record(
    const yap_log_record *record,
    bool include_timestamp,
    bool include_source,
    char *buffer,
    size_t capacity,
    size_t *written
);

void yap_log_console_write(
    const char *line,
    size_t length,
    yap_log_level level,
    bool use_colors
);

void yap_log_file_write(FILE *file, const char *line, size_t length);
