#include "core/project/project_internal.h"

#include "defines.h"


static b yap_project_relative_path_valid(const c *path) {
  if (path == NULL || path[0] == '\0' || path[0] == '/' ||
      path[0] == '\\') {
    return false;
  }

  const c *segment = path;

  for (const c *cursor = path;; ++cursor) {
    if (*cursor == '/' || *cursor == '\\' || *cursor == '\0') {
      const st length = (st)(cursor - segment);

      if (length == 0 ||
          (length == 1 && segment[0] == '.') ||
          (length == 2 && segment[0] == '.' && segment[1] == '.')) {
        return false;
      }

      if (*cursor == '\0') {
        break;
      }

      segment = cursor + 1;
    }

    if (*cursor == ':') {
      return false;
    }
  }

  return true;
}

b yap_project_plan_valid(const YAP_PROJECT_PLAN *plan) {
  if (plan == NULL || plan->root == NULL || plan->root[0] == '\0') {
    return false;
  }

  if (plan->operation_count > 0 && plan->operations == NULL) {
    return false;
  }

  for (st i = 0; i < plan->operation_count; ++i) {
    const YAP_PROJECT_OPERATION *operation = &plan->operations[i];

    if (!yap_project_relative_path_valid(operation->path)) {
      return false;
    }

    switch (operation->type) {
      case YAP_PROJECT_CREATE_DIRECTORY:
        break;

      case YAP_PROJECT_WRITE_FILE:
        if (operation->content == NULL) {
          return false;
        }
        break;

      default:
        return false;
    }
  }

  return true;
}
