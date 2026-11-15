#if !defined(_WIN32)
#define _POSIX_C_SOURCE 200809L
#endif

#include "logger_internal.h"

#include <stdio.h>
#include <time.h>

bool yap_log_timestamp(char *buffer, size_t capacity) {
  if (buffer == NULL || capacity == 0) {
    return false;
  }

  struct timespec now;

#if YAP_PLATFORM_WINDOWS
  if (timespec_get(&now, TIME_UTC) != TIME_UTC) {
    return false;
  }
#else
  if (clock_gettime(CLOCK_REALTIME, &now) != 0) {
    return false;
  }
#endif

  struct tm utc_time;

#if YAP_PLATFORM_WINDOWS
  if (gmtime_s(&utc_time, &now.tv_sec) != 0) {
    return false;
  }
#else
  if (gmtime_r(&now.tv_sec, &utc_time) == NULL) {
    return false;
  }
#endif

  const int count = snprintf(
      buffer,
      capacity,
      "%04d-%02d-%02dT%02d:%02d:%02d.%03ldZ",
      utc_time.tm_year + 1900,
      utc_time.tm_mon + 1,
      utc_time.tm_mday,
      utc_time.tm_hour,
      utc_time.tm_min,
      utc_time.tm_sec,
      now.tv_nsec / 1000000L
  );

  return count >= 0 && (size_t)count < capacity;
}
