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

  char project_identifier[65];
  char header_guard[65];

  for (size_t i = 0; name[i] != '\0'; ++i) {
    const char character = name[i] == '-' ? '_' : name[i];
    project_identifier[i] = character;
    header_guard[i] = character >= 'a' && character <= 'z'
                          ? (char)(character - 'a' + 'A')
                          : character;
  }

  project_identifier[strlen(name)] = '\0';
  header_guard[strlen(name)] = '\0';

  char header_filename[80];
  char source_filename[80];
  char project_header[512];
  char main_source[256];
  char project_source[512];

  const int header_filename_length =
      snprintf(header_filename, sizeof(header_filename), "include/%s.h", name);
  const int source_filename_length =
      snprintf(source_filename, sizeof(source_filename), "src/%s.c", name);
  const int project_header_length = snprintf(
      project_header, sizeof(project_header),
      "#ifndef %s_H\n"
      "#define %s_H\n"
      "\n"
      "#define PROJECT_NAME \"%s\"\n"
      "\n"
      "void %s(void);\n"
      "\n"
      "#endif // %s_H\n",
      header_guard, header_guard, name, project_identifier, header_guard);

  const int main_source_length = snprintf(
      main_source, sizeof(main_source),
      "#include \"%s.h\"\n"
      "\n"
      "int main(void) {\n"
      "  %s();\n"
      "  return 0;\n"
      "}\n",
      name, project_identifier);

  const int project_source_length = snprintf(
      project_source, sizeof(project_source),
      "#include \"%s.h\"\n"
      "\n"
      "#include <stdio.h>\n"
      "\n"
      "void %s(void) {\n"
      "  printf(\"Hello from %%s\\n\", PROJECT_NAME);\n"
      "}\n",
      name, project_identifier);

  if (header_filename_length < 0 ||
      (size_t)header_filename_length >= sizeof(header_filename) ||
      source_filename_length < 0 ||
      (size_t)source_filename_length >= sizeof(source_filename) ||
      project_header_length < 0 ||
      (size_t)project_header_length >= sizeof(project_header) ||
      main_source_length < 0 ||
      (size_t)main_source_length >= sizeof(main_source) ||
      project_source_length < 0 ||
      (size_t)project_source_length >= sizeof(project_source)) {
    return (YAP_PROJECT_RESULT){
        .status = YAP_PROJECT_PATH_TOO_LONG,
        .completed_operations = 0,
        .failed_operation = SIZE_MAX,
    };
  }

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
          .path = header_filename,
          .content = project_header,
      },
      {
          .type = YAP_PROJECT_WRITE_FILE,
          .path = "src/main.c",
          .content = main_source,
      },
      {
          .type = YAP_PROJECT_WRITE_FILE,
          .path = source_filename,
          .content = project_source,
      },
      {
          .type = YAP_PROJECT_WRITE_FILE,
          .path = "build.sh",
          .content = build_script,
          .executable = true,
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
