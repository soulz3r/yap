#include "logger_internal.h"

#include "defines.h"

#include <stdio.h>

void yap_log_file_write(FILE *file, const char *line, st length) {
  if (file == NULL || line == NULL) {
    return;
  }

  (void)fwrite(line, sizeof(char), length, file);
  (void)fflush(file);
}
