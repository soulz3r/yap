#include "logger_internal.h"

#include <stdio.h>

void yap_log_file_write(FILE *file, const char *line, size_t length) {
    if (file == NULL || line == NULL) {
        return;
    }

    (void)fwrite(line, sizeof(char), length, file);
    (void)fputc('\n', file);
    (void)fflush(file);
}
