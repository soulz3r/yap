#pragma once

#include "defines.h"

#include "core/logger/logger.h"

#include <stdarg.h>
#include <stdio.h>

#define YAP_LOG_MESSAGE_CAPACITY 2048
#define YAP_LOG_LINE_CAPACITY 4096
#define YAP_LOG_TIMESTAMP_CAPACITY 32

typedef struct YAP_LOG_RECORD {
  YAP_LOG_LEVEL level;
  const char *module;
  const char *file;
  const char *function;
  i32 line;

  char timestamp[YAP_LOG_TIMESTAMP_CAPACITY];
  char message[YAP_LOG_MESSAGE_CAPACITY];
} YAP_LOG_RECORD;

typedef struct YAP_LOG_STATE {
  b initialized;
  YAP_LOG_LEVEL min_level;

  b console_enabled;
  b file_enabled;
  b include_timestamp;
  b include_source;
  b use_colors;

  FILE *file;
} YAP_LOG_STATE;

extern YAP_LOG_STATE yap_logger;

const char *yap_log_level_name(YAP_LOG_LEVEL level);
b yap_log_timestamp(char *buffer, st capacity);
b yap_log_format_record(const YAP_LOG_RECORD *record,
                           b include_timestamp, b include_source,
                           char *buffer, st capacity, st *written);

void yap_log_console_write(const char *line, st length,
                           YAP_LOG_LEVEL level, b use_colors);
void yap_log_file_write(FILE *file, const char *line, st length);
