#include <stdio.h>

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

i32 cli_tree(int argc, char **argv){
  // TODO: write what tree command should be
  (void)argc;
  const char *path = argv[3];
  print_tree(path);
  return 0;
}
