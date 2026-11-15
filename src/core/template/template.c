#include "core/template/template.h"

#include <ctype.h>
#include <string.h>

static bool yap_template_append(char *output, size_t capacity, size_t *used,
                                const char *text, size_t length) {
  if (length > capacity - 1 - *used) {
    return false;
  }

  memcpy(output + *used, text, length);
  *used += length;
  output[*used] = '\0';
  return true;
}

static const char *yap_template_find_variable(
    const YAP_TEMPLATE_CONTEXT *context, const char *name, size_t name_length) {
  if (context == NULL) {
    return NULL;
  }

  for (size_t i = 0; i < context->variable_count; ++i) {
    const YAP_TEMPLATE_VARIABLE *variable = &context->variables[i];

    if (variable->name != NULL &&
        strlen(variable->name) == name_length &&
        memcmp(variable->name, name, name_length) == 0) {
      return variable->value;
    }
  }

  return NULL;
}

YAP_TEMPLATE_STATUS yap_template_render(const char *source,
                                         const YAP_TEMPLATE_CONTEXT *context,
                                         char *output, size_t capacity,
                                         size_t *written) {
  if (source == NULL || output == NULL || capacity == 0 || written == NULL ||
      (context != NULL && context->variable_count > 0 &&
       context->variables == NULL)) {
    return YAP_TEMPLATE_INVALID_ARGUMENT;
  }

  size_t used = 0;
  output[0] = '\0';

  const char *cursor = source;

  while (*cursor != '\0') {
    if (cursor[0] == '{' && cursor[1] == '{') {
      const char *expression = cursor + 2;
      const char *end = strstr(expression, "}}");

      if (end == NULL) {
        return YAP_TEMPLATE_UNTERMINATED_EXPRESSION;
      }

      while (expression < end && isspace((unsigned char)*expression)) {
        ++expression;
      }

      while (end > expression && isspace((unsigned char)end[-1])) {
        --end;
      }

      if (expression == end) {
        return YAP_TEMPLATE_UNKNOWN_VARIABLE;
      }

      const char *value =
          yap_template_find_variable(context, expression,
                                     (size_t)(end - expression));

      if (value == NULL) {
        return YAP_TEMPLATE_UNKNOWN_VARIABLE;
      }

      const size_t value_length = strlen(value);
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
