#if defined(YAP_PLATFORM_LINUX)

#include "cli/colors.h"
#include "cli/tree.h"
#include "defines.h"

#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

i32 file_count = 0;
i32 dir_count = 0;

static void print_entry(const char *path, bool isdir, int depth) {
  for (int i = 0; i < depth; i++) {
    printf("|   ");
  }

  if (depth > 0) {
    printf("+-- ");
  }

  if (isdir) {
    printf(YAP_GREEN "%s/\n" YAP_RESET, path);
  } else {
    printf(YAP_GREY "%s\n" YAP_RESET, path);
  }
}
static void print_count() {
  printf("\nTotal Directories : " YAP_AQUA "%d\n" YAP_RESET
         "Total Files       : " YAP_AQUA "%d\n" YAP_RESET,
         dir_count, file_count);
}

static int list_dir_l(const char *path, int depth) {
  DIR *dir = opendir(path);

  if (!dir) {
    perror("Can't open directory");
    return 0;
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
      dir_count++;
      print_entry(name, true, depth);

      size_t len = strlen(path) + strlen(name) + 2;
      char sub_path[len];

      snprintf(sub_path, len, "%s/%s", path, name);

      list_dir_l(sub_path, depth + 1);
    } else {
      file_count++;
      print_entry(name, false, depth);
    }
  }
  closedir(dir);
  return 1;
}

void print_tree(const char *path) {
  i32 is_true = list_dir_l(path, 0);
  if (is_true) {
    print_count();
  }
}
#endif /* if defie */
