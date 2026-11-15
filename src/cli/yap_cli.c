#include <stdio.h>

#include "defines.h"
#include <string.h>

#include "cli/cli_core.h"
#include "cli/colors.h"
#include "cli/help.h"
#include "cli/yap_cli.h"
#include "core/logger/logger.h"

static const struct command commands[] = {
    {"new", cli_new},     {"add", cli_add},   {"remove", cli_remove},
    {"check", cli_check}, {"run", cli_run},   {"clean", cli_clean},
    {"tree", cli_tree},   {"help", cli_help}, {"version", cli_version},
};

i32 yap_cli(i32 argc, c *argv[]) {

  // quit if no sub-commands
  if (argc < 2) {
    YAP_LOG_WARN("CLI", "No command was supplied");
    fprintf(stderr,
            "'yap' needs a command.\n"
            "Use 'yap --help' or 'yap -h' to see available commands.\n");
    return 1;
  }

  const c *name = argv[1];

  // global options.
  if (strcmp(name, "--version") == 0 || strcmp(name, "-v") == 0) {
    version();
    return 0;
  }

  if (strcmp(name, "--help") == 0 || strcmp(name, "-h") == 0) {
    help();
    return 0;
  }

  // dispacher
  for (st i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
    if (strcmp(name, commands[i].name) == 0) {
      return commands[i].cmd_func(argc - 1, argv + 1);
    }
  }

  YAP_LOG_ERROR("yap", "Unknown command: %s", name);
  return 1;
}
