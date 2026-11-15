#include "core/utils/make_fd.h"

#include "core/logger/logger.h"
#include "defines.h"

#include <stdio.h>
#include <string.h>

#if YAP_PLATFORM_WINDOWS
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

bool create_dir(const char *dir) {
  if (dir == NULL || dir[0] == '\0') {
    return false;
  }

#if YAP_PLATFORM_WINDOWS
  const int result = _mkdir(dir);
#else
  const int result = mkdir(dir, 0755);
#endif

  if (result != 0) {
    YAP_LOG_ERROR("FILESYSTEM", "Failed to create directory: %s", dir);
    return false;
  }

  return true;
}

bool create_file(const char *path) {
  if (path == NULL || path[0] == '\0') {
    return false;
  }

  FILE *file = fopen(path, "ab");
  if (file == NULL) {
    YAP_LOG_ERROR("FILESYSTEM", "Failed to create file: %s", path);
    return false;
  }

  if (fclose(file) != 0) {
    YAP_LOG_ERROR("FILESYSTEM", "Failed to close created file: %s", path);
    return false;
  }

  return true;
}

bool write_file(const char *path, const char *content) {
  if (path == NULL || path[0] == '\0' || content == NULL) {
    return false;
  }

  FILE *file = fopen(path, "w");
  if (file == NULL) {
    YAP_LOG_ERROR("FILESYSTEM", "Failed to open file for writing: %s", path);
    return false;
  }

  const size_t length = strlen(content);
  const size_t written = fwrite(content, sizeof(char), length, file);
  const int close_result = fclose(file);

  if (written != length || close_result != 0) {
    YAP_LOG_ERROR("FILESYSTEM", "Failed to write file: %s", path);
    return false;
  }

  return true;
}
