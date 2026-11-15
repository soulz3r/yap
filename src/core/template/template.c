#include "core/template/template.h"

#include "defines.h"

#include <ctype.h>
#include <string.h>

static b yap_template_append(c *output, st capacity, st *used,
                                const c *text, st length) {
  if (length > capacity - 1 - *used) {
    return false;
  }

  memcpy(output + *used, text, length);
  *used += length;
  output[*used] = '\0';
  return true;
}

static const c *yap_template_find_variable(
    const YAP_TEMPLATE_CONTEXT *context, const c *name, st name_length) {
  if (context == NULL) {
    return NULL;
  }

  for (st i = 0; i < context->variable_count; ++i) {
    const YAP_TEMPLATE_VARIABLE *variable = &context->variables[i];

    if (variable->name != NULL &&
        strlen(variable->name) == name_length &&
        memcmp(variable->name, name, name_length) == 0) {
      return variable->value;
    }
  }

  return NULL;
}

YAP_TEMPLATE_STATUS yap_template_render(const c *source,
                                         const YAP_TEMPLATE_CONTEXT *context,
                                         c *output, st capacity,
                                         st *written) {
  if (source == NULL || output == NULL || capacity == 0 || written == NULL ||
      (context != NULL && context->variable_count > 0 &&
       context->variables == NULL)) {
    return YAP_TEMPLATE_INVALID_ARGUMENT;
  }

  st used = 0;
  *written = 0;
  output[0] = '\0';

  const c *cursor = source;

  while (*cursor != '\0') {
    if (cursor[0] == '{' && cursor[1] == '{') {
      const c *expression = cursor + 2;
      const c *end = strstr(expression, "}}");

      if (end == NULL) {
        return YAP_TEMPLATE_UNTERMINATED_EXPRESSION;
      }

      while (expression < end && isspace((unsigned c)*expression)) {
        ++expression;
      }

      while (end > expression && isspace((unsigned c)end[-1])) {
        --end;
      }

      if (expression == end) {
        return YAP_TEMPLATE_UNKNOWN_VARIABLE;
      }

      const c *value =
          yap_template_find_variable(context, expression,
                                     (st)(end - expression));

      if (value == NULL) {
        return YAP_TEMPLATE_UNKNOWN_VARIABLE;
      }

      const st value_length = strlen(value);
      if (!yap_template_append(output, capacity, &used, value,
                               value_length)) {
        return YAP_TEMPLATE_BUFFER_TOO_SMALL;
      }

      cursor = strstr(cursor + 2, "}}") + 2;
      continue;
    }

    if (!yap_template_append(output, capacity, &used, cursor, 1)) {
      return YAP_TEMPLATE_BUFFER_TOO_SMALL;
    }

    ++cursor;
  }

  *written = used;
  return YAP_TEMPLATE_SUCCESS;
}
