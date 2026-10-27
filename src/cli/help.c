#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "cli/colors.h"
#include "cli/help.h"
#include "defines.h"

void version() { printf("Version: 0.0.1"); }

void help(void) {
  printf(
      YAP_GREEN YAP_BOLD
      "Yet Another Project (YAP)\n" YAP_RESET "\n"

      YAP_GREY "Usage:\n" YAP_RESET "  yap <command> [options]\n"
      "\n"

      YAP_GREY "Commands:\n" YAP_RESET "  " YAP_GREEN "new      " YAP_RESET
      "Create a new project\n"
      "  " YAP_GREEN "init     " YAP_RESET "Initialize an existing project\n"
      "  " YAP_GREEN "build    " YAP_RESET "Build the current project\n"
      "  " YAP_GREEN "run      " YAP_RESET "Build and run the current project\n"
      "  " YAP_GREEN "check    " YAP_RESET "Check project configuration\n"
      "  " YAP_GREEN "clean    " YAP_RESET "Remove build artifacts\n"
      "  " YAP_GREEN "tree     " YAP_RESET "Display project structure\n"
      "  " YAP_GREEN "help     " YAP_RESET "Display help information\n"
      "\n"

      YAP_GREY "Options:\n" YAP_RESET "  " YAP_YELLOW
      "-h, --help      " YAP_RESET "Show help\n"
      "  " YAP_YELLOW "-v, --version   " YAP_RESET "Show version information\n"
      "\n"

      YAP_GREY "Examples:\n" YAP_RESET "  yap new my-game\n"
      "  yap new my-game --template raylib\n"
      "  yap build --release\n"
      "  yap run\n"
      "  yap tree\n"
      "\n"

      YAP_GREY
      "Run 'yap help <command>' for command-specific help.\n" YAP_RESET);
}

void print_tree(const char *path) {
#if defined(YAP_PLATFORM_WINDOWS)
  char search_path[MAX_PATH];
  snprintf(search_path, sizeof(search_path), "%s\\*", path);

  WIN32_FIND_DATAA entry;
  HANDLE handle = FindFirstFileA(search_path, &entry);

  if (handle == INVALID_HANDLE_VALUE) {
    printf("Cannot access: %s\n", path);
    return;
  }

  do {
    if (strcmp(entry.cFileName, ".") == 0 ||
        strcmp(entry.cFileName, "..") == 0 ||
        strcmp(entry.cFileName, ".git") == 0) {
      continue;
    }

    printf("%s\n", entry.cFileName);

    if (entry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      char child_path[MAX_PATH];

      snprintf(child_path, sizeof(child_path), "%s\\%s", path, entry.cFileName);

      print_tree(child_path);
    }

  } while (FindNextFileA(handle, &entry));

  FindClose(handle);
#endif
}
