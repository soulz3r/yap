#pragma once

#include "defines.h"

/**
 * @brief Represents a CLI command and its cmd_func.
 *
 * Associates a command name with a function that handles its execution.
 *
 * @param name Name of the command.
 * @param cmd_func Pointer to the function that executes the command.
 */
struct command {
  const char *name;
  int (*cmd_func)(int argc, char *argv[]);
};

/**
 * @brief Entry point for YAP's command-line interface.
 *
 * Parses the command-line arguments and dispatches the requested command.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of strings containing the command-line arguments.
 * @return 0 on success, nonzero on failure.
 */
i32 yap_cli(int argc, char *argv[]);
