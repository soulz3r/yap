#include "core/project/project.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static bool yap_project_name_valid(const char *name) {
  if (name == NULL || name[0] == '\0' || strlen(name) > 64) {
    return false;
  }

  if (!((name[0] >= 'A' && name[0] <= 'Z') ||
        (name[0] >= 'a' && name[0] <= 'z') ||
        (name[0] >= '0' && name[0] <= '9'))) {
    return false;
  }

  for (const char *cursor = name; *cursor != '\0'; ++cursor) {
    const bool alphanumeric =
        (*cursor >= 'A' && *cursor <= 'Z') ||
        (*cursor >= 'a' && *cursor <= 'z') ||
        (*cursor >= '0' && *cursor <= '9');

    if (!alphanumeric && *cursor != '_' && *cursor != '-') {
      return false;
    }
  }

  static const char *reserved_names[] = {
      "CON", "PRN", "AUX", "NUL", "COM1", "COM2", "COM3",
      "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
      "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6",
      "LPT7", "LPT8", "LPT9",
  };

  for (size_t i = 0;
       i < sizeof(reserved_names) / sizeof(reserved_names[0]); ++i) {
    size_t j = 0;

    while (name[j] != '\0' && reserved_names[i][j] != '\0') {
      char left = name[j];
      char right = reserved_names[i][j];

      if (left >= 'a' && left <= 'z') {
        left = (char)(left - 'a' + 'A');
      }

      if (left != right) {
        break;
      }

      ++j;
    }

    if (name[j] == '\0' && reserved_names[i][j] == '\0') {
      return false;
    }
  }

  return true;
}

YAP_PROJECT_RESULT yap_project_generate_default(const char *name) {
  if (!yap_project_name_valid(name)) {
    return (YAP_PROJECT_RESULT){
        .status = YAP_PROJECT_INVALID_PLAN,
        .completed_operations = 0,
        .failed_operation = SIZE_MAX,
    };
  }

  static const char *main_source =
      "#include <stdio.h>\n"
      "\n"
      "int main(void) {\n"
      "  puts(\"Hello, world!\");\n"
      "  return 0;\n"
      "}\n";

  char build_script[4096];
  const int build_script_length = snprintf(
      build_script, sizeof(build_script),
      "#!/usr/bin/env bash\n"
      "\n"
      "set -eo pipefail\n"
      "\n"
      "mapfile -d '' c_files < <(find src -type f -name '*.c' -print0)\n"
      "\n"
      "if ((${#c_files[@]} == 0)); then\n"
      "  echo \"No C source files found.\" >&2\n"
      "  exit 1\n"
      "fi\n"
      "\n"
      "mkdir -p bin\n"
      "read -r -a extra_flags <<< \"${compilerFlags:-}\"\n"
      "\n"
      "clang -std=c23 -g -O0 -Wall -Wextra \\\n"
      "  -Iinclude -Isrc \\\n"
      "  \"${c_files[@]}\" \\\n"
      "  \"${extra_flags[@]}\" \\\n"
      "  -o \"./bin/%s\"\n"
      "\n"
      "echo \"Build succeeded: bin/%s\"\n",
      name, name);

  if (build_script_length < 0 ||
      (size_t)build_script_length >= sizeof(build_script)) {
    return (YAP_PROJECT_RESULT){
        .status = YAP_PROJECT_PATH_TOO_LONG,
        .completed_operations = 0,
        .failed_operation = SIZE_MAX,
    };
  }

  char build_batch[4096];
  const int build_batch_length = snprintf(
      build_batch, sizeof(build_batch),
      "@ECHO OFF\n"
      "SETLOCAL EnableDelayedExpansion\n"
      "\n"
      "SET \"appName=%s\"\n"
      "SET \"cFiles=\"\n"
      "\n"
      "FOR /R src %%%%f IN (*.c) DO (\n"
      "    SET \"cFiles=!cFiles! \"%%%%f\"\"\n"
      ")\n"
      "\n"
      "IF NOT DEFINED cFiles (\n"
      "    ECHO No C source files found.\n"
      "    EXIT /B 1\n"
      ")\n"
      "\n"
      "IF NOT EXIST bin MKDIR bin\n"
      "\n"
      "ECHO Building %%appName%%...\n"
      "clang -std=c23 -g -O0 -Wall -Wextra -Iinclude -Isrc %%cFiles%% %%compilerFlags%% -o \"bin\\%%appName%%.exe\"\n"
      "\n"
      "IF ERRORLEVEL 1 (\n"
      "    ECHO Build failed.\n"
      "    EXIT /B 1\n"
      ")\n"
      "\n"
      "ECHO Build succeeded: bin\\%%appName%%.exe\n"
      "EXIT /B 0\n",
      name);

  if (build_batch_length < 0 ||
      (size_t)build_batch_length >= sizeof(build_batch)) {
    return (YAP_PROJECT_RESULT){
        .status = YAP_PROJECT_PATH_TOO_LONG,
        .completed_operations = 0,
        .failed_operation = SIZE_MAX,
    };
  }

  static const char *compile_flags =
      "--std=c23\n"
      "-I\n"
      "include\n"
      "-I\n"
      "src\n";

  const YAP_PROJECT_OPERATION operations[] = {
      {
          .type = YAP_PROJECT_CREATE_DIRECTORY,
          .path = "include",
          .content = NULL,
      },
      {
          .type = YAP_PROJECT_CREATE_DIRECTORY,
          .path = "src",
          .content = NULL,
      },
      {
          .type = YAP_PROJECT_WRITE_FILE,
          .path = "src/main.c",
          .content = main_source,
      },
      {
          .type = YAP_PROJECT_WRITE_FILE,
          .path = "build.sh",
          .content = build_script,
      },
      {
          .type = YAP_PROJECT_WRITE_FILE,
          .path = "build.bat",
          .content = build_batch,
      },
      {
          .type = YAP_PROJECT_WRITE_FILE,
          .path = "compile_flags.txt",
          .content = compile_flags,
      },
  };

  const YAP_PROJECT_PLAN plan = {
      .root = name,
      .operations = operations,
      .operation_count = sizeof(operations) / sizeof(operations[0]),
  };

  return yap_project_generate(&plan);
}
