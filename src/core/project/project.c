#include "core/project/project.h"

#include "core/project/project_internal.h"
#include "core/utils/make_fd.h"

#include <stdint.h>
#include <stdio.h>

#define YAP_PROJECT_PATH_CAPACITY 4096

static bool yap_project_build_path(const char *root, const char *relative_path,
                                   char *buffer, size_t capacity) {
  const int length = snprintf(buffer, capacity, "%s/%s", root, relative_path);
  return length >= 0 && (size_t)length < capacity;
}

static YAP_PROJECT_RESULT yap_project_result(YAP_PROJECT_STATUS status,
                                             size_t completed,
                                             size_t failed) {
  return (YAP_PROJECT_RESULT){
      .status = status,
      .completed_operations = completed,
      .failed_operation = failed,
  };
}

YAP_PROJECT_RESULT yap_project_generate(const YAP_PROJECT_PLAN *plan) {
  if (!yap_project_plan_valid(plan)) {
    return yap_project_result(YAP_PROJECT_INVALID_PLAN, 0, SIZE_MAX);
  }

  if (!create_dir(plan->root)) {
    return yap_project_result(YAP_PROJECT_FILESYSTEM_ERROR, 0, SIZE_MAX);
  }

  char path[YAP_PROJECT_PATH_CAPACITY];
  size_t completed = 0;

  for (size_t i = 0; i < plan->operation_count; ++i) {
    const YAP_PROJECT_OPERATION *operation = &plan->operations[i];

    if (!yap_project_build_path(plan->root, operation->path, path,
                                sizeof(path))) {
      return yap_project_result(YAP_PROJECT_PATH_TOO_LONG, completed, i);
    }

    bool success = false;

    switch (operation->type) {
      case YAP_PROJECT_CREATE_DIRECTORY:
        success = create_dir(path);
        break;

      case YAP_PROJECT_WRITE_FILE:
        success = write_file(path, operation->content);
        break;

      default:
        return yap_project_result(YAP_PROJECT_INVALID_PLAN, completed, i);
    }

    if (!success) {
      return yap_project_result(YAP_PROJECT_FILESYSTEM_ERROR, completed, i);
    }

    ++completed;
  }

  return yap_project_result(YAP_PROJECT_SUCCESS, completed, SIZE_MAX);
}
