#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef enum YAP_PROJECT_OPERATION_TYPE {
  YAP_PROJECT_CREATE_DIRECTORY,
  YAP_PROJECT_WRITE_FILE
} YAP_PROJECT_OPERATION_TYPE;

typedef struct YAP_PROJECT_OPERATION {
  YAP_PROJECT_OPERATION_TYPE type;
  const char *path;
  const char *content;
  bool executable;
} YAP_PROJECT_OPERATION;

typedef struct YAP_PROJECT_PLAN {
  const char *root;
  const YAP_PROJECT_OPERATION *operations;
  size_t operation_count;
} YAP_PROJECT_PLAN;

typedef enum YAP_PROJECT_STATUS {
  YAP_PROJECT_SUCCESS,
  YAP_PROJECT_INVALID_PLAN,
  YAP_PROJECT_PATH_TOO_LONG,
  YAP_PROJECT_FILESYSTEM_ERROR
} YAP_PROJECT_STATUS;

typedef struct YAP_PROJECT_RESULT {
  YAP_PROJECT_STATUS status;
  size_t completed_operations;
  size_t failed_operation;
} YAP_PROJECT_RESULT;

/*
 * Executes a fully resolved project plan.
 *
 * The root directory must not already exist. Operation paths are relative
 * to root. Parent directories must appear before files that use them.
 * The caller retains ownership of the plan and all strings it references.
 */
YAP_PROJECT_RESULT yap_project_generate(const YAP_PROJECT_PLAN *plan);

/* Creates the built-in default C project template in a new directory. */
YAP_PROJECT_RESULT yap_project_generate_default(const char *name);
