#include "logger_internal.h"

#include "defines.h"

#include <stdio.h>

v yap_log_file_write(FILE *file, const c *line, st length) {
  if (file == NULL || line == NULL) {
    return;
  }

  (v)fwrite(line, sizeof(c), length, file);
  (v)fflush(file);
}
