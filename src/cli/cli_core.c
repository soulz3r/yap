#include <direct.h>
#include <stdio.h>
#include <stdlib.h>

#include "cli/cli_core.h"
#include "cli/help.h"

i32 cli_new(int argc, char **argv) {
  // TODO: write what new does
  (void)argc;
  (void)argv;
  printf("Creating new project...\n");
  return 0;
}

i32 cli_add(int argc, char **argv) {
  // TODO: write what add does
  (void)argc;
  (void)argv;
  printf("Adding something...\n");
  return 0;
}

i32 cli_remove(int argc, char **argv) {
  // TODO: write what remove does
  (void)argc;
  (void)argv;
  printf("Removing something\n");
  return 0;
}

i32 cli_check(int argc, char **argv) {
  // TODO: write what check does
  (void)argc;
  (void)argv;
  printf("Checking something\n");
  return 0;
}

i32 cli_run(int argc, char **argv) {
  // TODO: write what running does
  (void)argc;
  (void)argv;
  printf("Running the project\n");
  return 0;
}

i32 cli_clean(int argc, char **argv) {
  // TODO: write what clean does
  (void)argc;
  (void)argv;
  printf("Cleaning something\n");
  return 0;
}

i32 cli_tree(int argc, char **argv) {
  char pwd[_MAX_PATH];
  const char *path;

  if (argc < 2) {
    if (_getcwd(pwd, sizeof(pwd)) == NULL) {
      perror("_getcwd");
      return 1;
    }

    path = pwd;
  } else {
    path = argv[1];
  }

  print_tree(path);
  return 0;
}

i32 cli_help(int argc, char **argv) {
  // TODO: write what tree command should be
  (void)argc;
  (void)argv;
  help();
  return 0;
}

i32 cli_version(int argc, char **argv) {
  // TODO: write what tree command should be
  (void)argc;
  (void)argv;
  version();
  return 0;
}
