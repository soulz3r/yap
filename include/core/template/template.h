#pragma once

#include "defines.h"


typedef struct YAP_TEMPLATE_VARIABLE {
  const c *name;
  const c *value;
} YAP_TEMPLATE_VARIABLE;

typedef struct YAP_TEMPLATE_CONTEXT {
  const YAP_TEMPLATE_VARIABLE *variables;
  st variable_count;
} YAP_TEMPLATE_CONTEXT;

typedef enum YAP_TEMPLATE_STATUS {
  YAP_TEMPLATE_SUCCESS,
  YAP_TEMPLATE_INVALID_ARGUMENT,
  YAP_TEMPLATE_UNTERMINATED_EXPRESSION,
  YAP_TEMPLATE_UNKNOWN_VARIABLE,
  YAP_TEMPLATE_BUFFER_TOO_SMALL
} YAP_TEMPLATE_STATUS;

/*
 * Replaces {{variable.name}} expressions using the supplied context.
 * The output is always null-terminated when capacity is nonzero.
 * Expressions not found in the context are reported as errors.
 */
YAP_TEMPLATE_STATUS yap_template_render(const c *source,
                                         const YAP_TEMPLATE_CONTEXT *context,
                                         c *output, st capacity,
                                         st *written);
