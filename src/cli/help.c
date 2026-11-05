#include <dirent.h>
#include <stdio.h>
#include <string.h>

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

void print_entry(char *path, bool isdir, int indent) {
  if (indent == 0 && isdir)
    printf(YAP_GREEN "%s/\n" YAP_RESET, path);
  else if (indent > 0 && isdir) {
    printf("%*c", indent * 4, ' ');
    printf(YAP_GREEN "%s/\n" YAP_RESET, path);
  } else if (indent == 0) {
    printf(YAP_GREY "%s\n" YAP_RESET, path);
  } else {
    printf("%*c", indent * 4, ' ');
    printf(YAP_GREY "%s\n" YAP_RESET, path);
  }
}

int list_dir(const char *path, int depth) {
  DIR *dir = opendir(path);
  if (!dir) {
    perror("Cant open dir");
    --depth;
    return 1;
  }
  struct dirent *dp;

  // read dir entries
  while ((dp = readdir(dir)) != NULL) {
    char *name = dp->d_name;
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0 ||
        strcmp(name, ".git") == 0) {
      continue;
    }

    // walk sub dirs
    bool isdir = dp->d_type == DT_DIR ? true : false;
    if (isdir) {
      print_entry(name, 1, depth);
      size_t len = strlen(path) + strlen(name) + 2;
      char sub_path[len];
      snprintf(sub_path, len, "%s/%s", path, name);
      list_dir(sub_path, depth + 1);
    } else {
      print_entry(name, 0, depth);
    }
  }
  closedir(dir);
  return 0;
}

void print_tree(const char *path) {
#if defined(YAP_PLATFORM_LINUX)
  list_dir(path, 0);
#endif
}
