#include "core/project/project.h"

#include "core/project/project_internal.h"
#include "core/utils/make_fd.h"
#include "defines.h"

#include <stdint.h>
#include <stdio.h>

#if !YAP_PLATFORM_WINDOWS
#include <sys/stat.h>
#endif

#define YAP_PROJECT_PATH_CAPACITY 4096

static b yap_project_build_path(const c *root, const c *relative_path,
                                   c *buffer, st capacity) {
  const i32 length = snprintf(buffer, capacity, "%s/%s", root, relative_path);
  return length >= 0 && (st)length < capacity;
}

static b yap_project_set_executable(const c *path) {
#if YAP_PLATFORM_WINDOWS
  (v)path;
  return true;
#else
  return chmod(path, S_IRUSR | S_IWUSR | S_IXUSR |
                         S_IRGRP | S_IXGRP |
                         S_IROTH | S_IXOTH) == 0;
#endif
}

static YAP_PROJECT_RESULT yap_project_result(YAP_PROJECT_STATUS status,
                                             st completed,
                                             st failed) {
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

  c path[YAP_PROJECT_PATH_CAPACITY];
  st completed = 0;

  for (st i = 0; i < plan->operation_count; ++i) {
    const YAP_PROJECT_OPERATION *operation = &plan->operations[i];

    if (!yap_project_build_path(plan->root, operation->path, path,
                                sizeof(path))) {
      return yap_project_result(YAP_PROJECT_PATH_TOO_LONG, completed, i);
    }

    b success = false;

    switch (operation->type) {
      case YAP_PROJECT_CREATE_DIRECTORY:
        success = create_dir(path);
        break;

      case YAP_PROJECT_WRITE_FILE:
        success = write_file(path, operation->content);

        if (success && operation->executable) {
          success = yap_project_set_executable(path);
        }
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
