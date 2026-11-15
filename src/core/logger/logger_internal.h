#pragma once

#include "core/logger/logger.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#define YAP_LOG_MESSAGE_CAPACITY 2048
#define YAP_LOG_LINE_CAPACITY 4096
#define YAP_LOG_TIMESTAMP_CAPACITY 32

typedef struct YAP_LOG_RECORD {
  YAP_LOG_LEVEL level;
  const char *module;
  const char *file;
  const char *function;
  int line;

  char timestamp[YAP_LOG_TIMESTAMP_CAPACITY];
  char message[YAP_LOG_MESSAGE_CAPACITY];
} YAP_LOG_RECORD;

typedef struct YAP_LOG_STATE {
  bool initialized;
  YAP_LOG_LEVEL min_level;

  bool console_enabled;
  bool file_enabled;
  bool include_timestamp;
  bool include_source;
  bool use_colors;

  FILE *filel level);
  bool yap_log_timestamp(char *buffer, size_t capacity);
  bool yap_log_format_record(const YAP_LOG_RECORD *record,
                             bool include_timestamp, bool include_source,
                             char *buffer, size_t capacity, size_t *written);

  void yap_log_console_write(const char *line, size_t length,
                             YAP_LOG_LEVEL level, bool use_colors);

  void yap_log_file_write(FILE *file, const char *line, size_t length);
