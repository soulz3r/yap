#include "defines.h"

#include <stdio.h>

#include "cli/colors.h"
#include "cli/help.h"

v version() { printf("Version: 0.0.1"); }

v help(v) {
  printf(YAP_GREEN YAP_BOLD
         "Yet Another Project (YAP)\n" YAP_RESET "\n"

         YAP_GREY "Usage:\n" YAP_RESET "  yap <command> [options]\n"
         "\n"

         "  " YAP_GREEN "new      " YAP_RESET
         "Create a project from a template\n"
         "  " YAP_GREEN "tree     " YAP_RESET "Display project structure\n"
         "  " YAP_GREEN "help     " YAP_RESET "Display help information\n"
         "\n"

         YAP_GREY "New project:\n" YAP_RESET
         "  yap new default <project-name>\n"
         "\n"

         YAP_GREY "Options:\n" YAP_RESET "  " YAP_YELLOW
         "-h, --help      " YAP_RESET "Show help\n"
         "  " YAP_YELLOW "-v, --version   " YAP_RESET
         "Show version information\n"
         "\n");
}
