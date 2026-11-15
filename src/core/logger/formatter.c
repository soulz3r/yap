#include "logger_internal.h"

#include <stdarg.h>
#include <stdio.h>

static bool append(char *buffer, size_t capacity, size_t *used,
                   const char *format, ...) {
  if (*used >= capacity) {
    return false;
  }

  va_list args;
  va_start(args, format);
  const int count = vsnprintf(buffer + *used, capacity - *used, format, args);
  va_end(args);

  if (count < 0 || (size_t)count >= capacity - *used) {
    return false;
  }

  *used += (size_t)count;
  return true;
}

bool yap_log_format_record(const yap_log_record *record, bool include_timestamp,
                           bool include_source, char *buffer, size_t capacity,
                           size_t *written) {
  if (record == NULL || buffer == NULL || capacity == 0 || written == NULL) {
    return false;
  }

  size_t used = 0;
  buffer[0] = '\0';

  if (!append(buffer, capacity, &used, "[%-5s]",
              yap_log_level_name(record->level))) {
    return false;
  }

  if (include_timestamp &&
      !append(buffer, capacity, &used, " [%s]", record->timestamp)) {
    return false;
  }

  if (include_source && record->file != NULL) {
    if (!append(buffer, capacity, &used, " %s:%d",
                record->file, record->line)) {
      return false;
    }

    if (record->function != NULL &&
        !append(buffer, capacity, &used, " (%s)", record->function)) {
      return false;
    }
  }

  if (!append(buffer, capacity, &used, " %s\n", record->message)) {
    return false;
  }

  *written = used;
  return true;
}
