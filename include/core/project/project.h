#pragma once

#include "defines.h"


typedef enum YAP_PROJECT_OPERATION_TYPE {
  YAP_PROJECT_CREATE_DIRECTORY,
  YAP_PROJECT_WRITE_FILE
} YAP_PROJECT_OPERATION_TYPE;

typedef struct YAP_PROJECT_OPERATION {
  YAP_PROJECT_OPERATION_TYPE type;
  const c *path;
  const c *content;
  b executable;
} YAP_PROJECT_OPERATION;

typedef struct YAP_PROJECT_PLAN {
  const c *root;
  const YAP_PROJECT_OPERATION *operations;
  st operation_count;
} YAP_PROJECT_PLAN;

/* User input shared by every project template. */
typedef struct YAP_PROJECT_CONFIG {
  const c *name;
  const c *template_name;
} YAP_PROJECT_CONFIG;

typedef enum YAP_PROJECT_STATUS {
  YAP_PROJECT_SUCCESS,
  YAP_PROJECT_INVALID_PLAN,
  YAP_PROJECT_PATH_TOO_LONG,
  YAP_PROJECT_FILESYSTEM_ERROR,
  YAP_PROJECT_INVALID_CONFIG,
  YAP_PROJECT_UNKNOWN_TEMPLATE
} YAP_PROJECT_STATUS;

typedef struct YAP_PROJECT_RESULT {
  YAP_PROJECT_STATUS status;
  st completed_operations;
  st failed_operation;
} YAP_PROJECT_RESULT;

/*
 * Executes a fully resolved project plan.
 *
 * The root directory must not already exist. Operation paths are relative
 * to root. Parent directories must appear before files that use them.
 * The caller retains ownership of the plan and all strings it references.
 */
YAP_PROJECT_RESULT yap_project_generate(const YAP_PROJECT_PLAN *plan);

/* Selects a built-in template and generates a project from its config. */
YAP_PROJECT_RESULT yap_project_create(const YAP_PROJECT_CONFIG *config);

/* Convenience wrapper retained for the existing `new default` command. */
YAP_PROJECT_RESULT yap_project_generate_default(const c *name);
