# Project-generation foundations

This document describes the project-creation coordinator, project-plan executor, template renderer, and the first built-in project template in YAP. The `new` command uses the coordinator to select the `default` C template. External template packages are not implemented.

## Current boundary

The project subsystem currently has four related pieces:

1. The project-creation coordinator accepts a shared project configuration and selects a built-in template.
2. The built-in default template defines the starter C project layout and prepares its operation plan.
3. The project-plan executor performs an ordered list of filesystem operations.
4. The template renderer replaces named variables in caller-provided text, but is not yet used by the default template.

The built-in `default` template is connected to `yap new default <project-name>` through a compatibility wrapper. YAP does not yet load external template packages or automatically convert rendered template files into a project plan.

## Project-creation coordinator

The public entry point is:

```c
YAP_PROJECT_RESULT yap_project_create(const YAP_PROJECT_CONFIG *config);
```

A configuration contains the project name and the selected template name:

```c
const YAP_PROJECT_CONFIG config = {
    .name = "myproject",
    .template_name = "default",
};

const YAP_PROJECT_RESULT result = yap_project_create(&config);
```

The coordinator uses a small built-in template registry. It resolves the requested template and delegates to that template provider. It does not perform filesystem operations. Adding a built-in template means adding a provider and registering it; the plan executor does not need to change.

The existing `yap_project_generate_default(name)` API remains as a convenience wrapper, so the current CLI command does not need to know about the configuration API yet.

## Project plan

Public header:

```c
#include "core/project/project.h"
```

A plan uses three structures:

- `YAP_PROJECT_OPERATION`: one operation and its relative path.
- `YAP_PROJECT_PLAN`: output root and ordered operation array.
- `YAP_PROJECT_RESULT`: status and execution progress.

### Operation types

| Type | Required fields | Effect |
|---|---|---|
| `YAP_PROJECT_CREATE_DIRECTORY` | `path` | Create a directory under the plan root |
| `YAP_PROJECT_WRITE_FILE` | `path` and `content` | Write text content under the plan root |

Paths are relative to the root. Validation rejects absolute paths, empty segments, `.`, `..`, and colon characters in path segments.

### Execution rules

The public function is:

```c
YAP_PROJECT_RESULT yap_project_generate(const YAP_PROJECT_PLAN *plan);
```

The executor:

1. Validates the plan.
2. Creates the root directory.
3. Walks operations in array order.
4. Joins the root with each relative path.
5. Executes directory creation or file writing.
6. Returns status and operation progress.

Contract:

- The root directory must not already exist.
- The caller must list parent directories before files that use them.
- The caller retains ownership of the plan and its strings.
- Operations execute in order.
- The executor does not decide which files belong in the project.

### Example plan

This illustrates how a caller can construct a project plan directly. The `yap new default <project-name>` command uses the same executor with its own built-in operation list:

```c
#include "core/project/project.h"

int main(void) {
  const YAP_PROJECT_OPERATION operations[] = {
      {
          .type = YAP_PROJECT_CREATE_DIRECTORY,
          .path = "src",
          .content = NULL,
      },
      {
          .type = YAP_PROJECT_CREATE_DIRECTORY,
          .path = "include",
          .content = NULL,
      },
      {
          .type = YAP_PROJECT_WRITE_FILE,
          .path = "src/main.c",
          .content = "#include <stdio.h>\n\nint main(void) {\n"
                     "  puts(\"Hello, world!\");\n  return 0;\n}\n",
      },
      {
          .type = YAP_PROJECT_WRITE_FILE,
          .path = "README.md",
          .content = "# Example project\n",
      },
  };

  const YAP_PROJECT_PLAN plan = {
      .root = "example",
      .operations = operations,
      .operation_count = sizeof(operations) / sizeof(operations[0]),
  };

  const YAP_PROJECT_RESULT result = yap_project_generate(&plan);
  return result.status == YAP_PROJECT_SUCCESS ? 0 : 1;
}
```

The caller owns the arrays and strings for the duration of the call. The API does not take ownership.

### Result status

| Status | Meaning |
|---|---|
| `YAP_PROJECT_SUCCESS` | Every operation completed |
| `YAP_PROJECT_INVALID_PLAN` | The plan or an operation was invalid |
| `YAP_PROJECT_PATH_TOO_LONG` | A joined path exceeded the internal path buffer |
| `YAP_PROJECT_FILESYSTEM_ERROR` | A filesystem operation failed |
| `YAP_PROJECT_INVALID_CONFIG` | The project configuration or template name is missing |
| `YAP_PROJECT_UNKNOWN_TEMPLATE` | No registered template matches the requested name |

The result also contains:

- `completed_operations`: number of completed operations.
- `failed_operation`: index of the failed operation.

When failure occurs before the operation loop, such as invalid plan or failure to create the root, `failed_operation` is `SIZE_MAX`. It is also `SIZE_MAX` on success.

### Partial output

The executor is not transactional. If a later operation fails, earlier directories and files remain. It does not roll back partial output.

Callers must report partial progress accurately and must not claim that a failed generation left the filesystem unchanged.

## Template renderer

Public header:

```c
#include "core/template/template.h"
```

The renderer replaces expressions such as:

```text
Project: {{project.name}}
Language: {{project.language}}
```

The caller supplies a `YAP_TEMPLATE_CONTEXT` containing an array of `YAP_TEMPLATE_VARIABLE` name/value pairs. The renderer writes to a caller-provided output buffer and does not allocate its own buffer.

### Example

```c
#include <stdio.h>

#include "core/template/template.h"

int main(void) {
  const YAP_TEMPLATE_VARIABLE variables[] = {
      {.name = "project.name", .value = "hello"},
      {.name = "project.language", .value = "C"},
  };

  const YAP_TEMPLATE_CONTEXT context = {
      .variables = variables,
      .variable_count = sizeof(variables) / sizeof(variables[0]),
  };

  const char *source =
      "Project: {{project.name}}\n"
      "Language: {{project.language}}\n";

  char output[256];
  size_t written = 0;

  const YAP_TEMPLATE_STATUS status =
      yap_template_render(source, &context, output, sizeof(output), &written);

  if (status != YAP_TEMPLATE_SUCCESS) {
    return 1;
  }

  printf("%s", output);
  return 0;
}
```

Whitespace around an expression name is ignored. For example, `{{ project.name }}` resolves the same variable as `{{project.name}}`.

### Status values

| Status | Meaning |
|---|---|
| `YAP_TEMPLATE_SUCCESS` | Rendering completed |
| `YAP_TEMPLATE_INVALID_ARGUMENT` | Required pointer or capacity was invalid |
| `YAP_TEMPLATE_UNTERMINATED_EXPRESSION` | An opening `{{` has no closing `}}` |
| `YAP_TEMPLATE_UNKNOWN_VARIABLE` | The expression is empty or no matching variable exists |
| `YAP_TEMPLATE_BUFFER_TOO_SMALL` | The rendered output does not fit |

The `written` output parameter is reset to zero before rendering and receives the number of output characters on success. When capacity is nonzero, the output is null-terminated.

### Current renderer limitations

The renderer supports variable substitution only. It does not support:

- conditionals;
- loops;
- template inheritance or includes;
- loading template files from disk;
- project-specific variable validation;
- file or directory creation.

Unknown variables are errors rather than silently becoming empty strings.

## How the components relate today

```text
Template source + variable context
              |
              v
     yap_template_render()
              |
              v
        Rendered text
              |
              | caller assembles plan
              v
        YAP_PROJECT_PLAN
              |
              v
     yap_project_generate()
              |
              v
          Filesystem
```

The caller currently performs the coordination between rendering and plan construction. The built-in default project template does not use the text renderer yet; it supplies its starter file contents directly to the project plan.

## Intended future workflow

The project configuration type and built-in template coordinator now provide the shared core entry point. The current CLI still calls the compatibility wrapper for the default template, while a future TUI can call `yap_project_create()` with the same configuration type.

The complete workflow is not implemented yet. There is no external template package format or loader, and the default template currently builds its operation list directly rather than using the text renderer. The current `new` command supports only the built-in `default` template.

## Safety considerations

The plan validator rejects common absolute-path and traversal forms. This is useful validation, but it is not a complete filesystem security boundary.

When extending project generation:

- Validate project names and destination paths before writing.
- Account for platform-specific path rules and reserved names.
- Consider symbolic links when writing into untrusted locations.
- Define behavior when the destination already exists.
- Make overwrite behavior explicit.
- Report partial execution and cleanup behavior accurately.
- Keep template content separate from destination paths.
