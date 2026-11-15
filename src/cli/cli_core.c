#include <stdio.h>

#include "defines.h"
#include <string.h>

#include "cli/cli_core.h"
#include "cli/help.h"
#include "cli/tree.h"
#include "core/logger/logger.h"
#include "core/project/project.h"

i32 cli_new(i32 argc, char **argv) {
  if (argc != 3 || strcmp(argv[1], "default") != 0) {
    fprintf(stderr, "Usage: yap new default <project-name>\n");
    return 1;
  }

  const char *project_name = argv[2];
  const YAP_PROJECT_RESULT result =
      yap_project_generate_default(project_name);

  if (result.status != YAP_PROJECT_SUCCESS) {
    if (result.status == YAP_PROJECT_INVALID_PLAN) {
      fprintf(stderr,
              "Invalid project name. Use 1-64 characters: letters, "
              "numbers, '_' or '-'.\n");
    } else {
      fprintf(stderr, "Failed to create project '%s'.\n", project_name);
    }

    YAP_LOG_ERROR("PROJECT", "Default project generation failed: %s",
                  project_name);
    return 1;
  }

  printf("Created project '%s'.\n", project_name);
  printf("  cd %s\n", project_name);
  printf("  bash build.sh (Linux)\n");
  printf("  .\\build.bat (Windows)\n");
  return 0;
}

i32 cli_add(i32 argc, char **argv) {
  // TODO: write what add does
  (void)argc;
  (void)argv;
  printf("Adding something...\n");
  return 0;
}

i32 cli_remove(i32 argc, char **argv) {
  // TODO: write what remove does
  (void)argc;
  (void)argv;
  printf("Removing something\n");
  return 0;
}

i32 cli_check(i32 argc, char **argv) {
  // TODO: write what check does
  (void)argc;
  (void)argv;
  printf("Checking something\n");
  return 0;
}

i32 cli_run(i32 argc, char **argv) {
  // TODO: write what running does
  (void)argc;
  (void)argv;
  printf("Running the project\n");
  return 0;
}

i32 cli_clean(i32 argc, char **argv) {
  // TODO: write what clean does
  (void)argc;
  (void)argv;
  printf("Cleaning something\n");
  return 0;
}

i32 cli_tree(i32 argc, char **argv) {
  (void)argc;
  (void)argv;

  const char *path = ".";
  print_tree(path);

  return 0;
}

i32 cli_help(i32 argc, char **argv) {
  // TODO: write what tree command should be
  (void)argc;
  (void)argv;
  help();
  return 0;
}

i32 cli_version(i32 argc, char **argv) {
  // TODO: write what tree command should be
  (void)argc;
  (void)argv;
  version();
  return 0;
}
