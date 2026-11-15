#include "logger_internal.h"

#include "defines.h"

#include <stdarg.h>
#include <stdio.h>

static b append(char *buffer, st capacity, st *used,
                   const char *format, ...) {
  if (*used >= capacity) {
    return false;
  }

  va_list args;
  va_start(args, format);
  const i32 count = vsnprintf(buffer + *used, capacity - *used, format, args);
  va_end(args);

  if (count < 0 || (st)count >= capacity - *used) {
    return false;
  }

  *used += (st)count;
  return true;
}

b yap_log_format_record(const YAP_LOG_RECORD *record, b include_timestamp,
                           b include_source, char *buffer, st capacity,
                           st *written) {
  if (record == NULL || buffer == NULL || capacity == 0 || written == NULL) {
    return false;
  }

  st used = 0;
  buffer[0] = '\0';

  if (!append(buffer, capacity, &used, "[%s]",
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
