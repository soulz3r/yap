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

static FILE *yap_open_file(const char *path, const char *mode) {
#if YAP_PLATFORM_WINDOWS
  FILE *file = NULL;

  if (fopen_s(&file, path, mode) != 0) {
    return NULL;
  }

  return file;
#else
  return fopen(path, mode);
#endif
}

b create_dir(const char *dir) {
  if (dir == NULL || dir[0] == '\0') {
    return false;
  }

#if YAP_PLATFORM_WINDOWS
  const i32 result = _mkdir(dir);
#else
  const i32 result = mkdir(dir, 0755);
#endif

  if (result != 0) {
    YAP_LOG_ERROR("FILESYSTEM", "Failed to create directory: %s", dir);
    return false;
  }

  return true;
}

b create_file(const char *path) {
  if (path == NULL || path[0] == '\0') {
    return false;
  }

  FILE *file = yap_open_file(path, "ab");
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

b write_file(const char *path, const char *content) {
  if (path == NULL || path[0] == '\0' || content == NULL) {
    return false;
  }

  FILE *file = yap_open_file(path, "w");
  if (file == NULL) {
    YAP_LOG_ERROR("FILESYSTEM", "Failed to open file for writing: %s", path);
    return false;
  }

  const st length = strlen(content);
  const st written = fwrite(content, sizeof(char), length, file);
  const i32 close_result = fclose(file);

  if (written != length || close_result != 0) {
    YAP_LOG_ERROR("FILESYSTEM", "Failed to write file: %s", path);
    return false;
  }

  return true;
}
