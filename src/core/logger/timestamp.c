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

  struct tm local_time;

#if YAP_PLATFORM_WINDOWS
  if (localtime_s(&local_time, &now.tv_sec) != 0) {
    return false;
  }
#else
  if (localtime_r(&now.tv_sec, &local_time) == NULL) {
    return false;
  }
#endif

  const int count = snprintf(
      buffer,
      capacity,
      "%02d:%02d:%02d.%03ld",
      local_time.tm_hour,
      local_time.tm_min,
      local_time.tm_sec,
      now.tv_nsec / 1000000L
  );

  return count >= 0 && (size_t)count < capacity;
}
