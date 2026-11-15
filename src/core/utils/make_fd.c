#include "core/utils/make_fd.h"
#include "core/logger/logger.h"

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

bool create_dir(const char *dir) {
  if (mkdir(dir, 0755) != 0) {
    YAP_LOG_ERROR("Internal", "Cant make the dir: %s\n", &dir);
    return false;
  }
  return true;
};

bool create_file(const char *path) {
  if (path == NULL || path[0] == '\0') {
    return false;
  }

  FILE *file = fopen(path, "ab");

  if (file == NULL) {
    YAP_LOG_ERROR("FILESYSTEM", "Failed to create file: %s", path);
    return false;
  }

  fclose(file);
  return true;
}

bool write_file(const char *path, const char *content) {
  if (path == NULL || content == NULL) {
    return false;
  }

  FILE *file = fopen(path, "w");

  if (file == NULL) {
    YAP_LOG_ERROR("FILESYSTEM", "Failed to open file: %s", path);
    return false;
  }

  const size_t length = strlen(content);
  const size_t written = fwrite(content, 1, length, file);

  const bool success = written == length && fclose(file) == 0;

  return success;
}
