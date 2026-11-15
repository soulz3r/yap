#include "core/project/project.h"

#include "defines.h"

#include "core/project/project_create_internal.h"

#include <stdint.h>
#include <string.h>

typedef YAP_PROJECT_RESULT (*YAP_PROJECT_TEMPLATE_GENERATOR)(
    const YAP_PROJECT_CONFIG *config);

typedef struct YAP_PROJECT_TEMPLATE_ENTRY {
  const c *name;
  YAP_PROJECT_TEMPLATE_GENERATOR generate;
} YAP_PROJECT_TEMPLATE_ENTRY;

static const YAP_PROJECT_TEMPLATE_ENTRY templates[] = {
    {
        .name = "default",
        .generate = yap_project_template_default,
    },
};

static YAP_PROJECT_RESULT yap_project_create_result(YAP_PROJECT_STATUS status) {
  return (YAP_PROJECT_RESULT){
      .status = status,
      .completed_operations = 0,
      .failed_operation = SIZE_MAX,
  };
}

YAP_PROJECT_RESULT yap_project_create(const YAP_PROJECT_CONFIG *config) {
  if (config == NULL || config->template_name == NULL ||
      config->template_name[0] == '\0') {
    return yap_project_create_result(YAP_PROJECT_INVALID_CONFIG);
  }

  for (st i = 0; i < sizeof(templates) / sizeof(templates[0]); ++i) {
    if (strcmp(config->template_name, templates[i].name) == 0) {
      return templates[i].generate(config);
    }
  }

  return yap_project_create_result(YAP_PROJECT_UNKNOWN_TEMPLATE);
}

/*
 * Compatibility wrapper for the original default-template API.
 * The CLI can keep using this until it accepts a full project config.
 */
YAP_PROJECT_RESULT yap_project_generate_default(const c *name) {
  const YAP_PROJECT_CONFIG config = {
      .name = name,
      .template_name = "default",
  };

  return yap_project_create(&config);
}
