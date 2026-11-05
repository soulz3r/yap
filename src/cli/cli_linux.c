#if defined(YAP_PLATFORM_LINUX)
#include "cli/colors.h"
#include "cli/tree.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <dirent.h>

static void print_entry(const char *path, bool isdir, int indent) {
  if (indent > 0) {
    printf("%*c", indent * 4, ' ');
  }

  if (isdir) {
    printf(YAP_GREEN "%s/\n" YAP_RESET, path);
  } else {
    printf(YAP_GREY "%s\n" YAP_RESET, path);
  }
}

static int list_dir_l(const char *path, int depth) {
  DIR *dir = opendir(path);

  if (!dir) {
    perror("Can't open directory");
    return 1;
  }

  struct dirent *dp;

  while ((dp = readdir(dir)) != NULL) {
    const char *name = dp->d_name;

    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0 ||
        strcmp(name, ".git") == 0) {
      continue;
    }

    bool isdir = dp->d_type == DT_DIR;

    if (isdir) {
      print_entry(name, true, depth);

      size_t len = strlen(path) + strlen(name) + 2;
      char sub_path[len];

      snprintf(sub_path, len, "%s/%s", path, name);

      list_dir_l(sub_path, depth + 1);
    } else {
      print_entry(name, false, depth);
    }
  }

  closedir(dir);
  return 0;
}

void print_tree(const char *path) { list_dir_l(path, 0); }

#endif
