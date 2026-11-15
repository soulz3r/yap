#if defined(YAP_PLATFORM_LINUX)

#include "cli/colors.h"
#include "cli/tree.h"
#include "defines.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

st file_count = 0;
st dir_count = 0;

static b is_directory(const char *path) {
  struct stat info;
  return stat(path, &info) == 0 && S_ISDIR(info.st_mode);
}

static i32 compare_entries(const void *a, const void *b) {
  const char *const *x = a;
  const char *const *y = b;

  return strcmp(*x, *y);
}

static void print_entry(const char *name, b isdir, i32 depth,
                        const b *last, b is_last) {
  for (i32 i = 0; i < depth; i++) {
    printf(last[i] ? "    " : YAP_DIM "│   " YAP_RESET);
  }

  printf(is_last ? YAP_DIM "└── " YAP_RESET : YAP_DIM "├── " YAP_RESET);

  if (isdir)
    printf(YAP_BLUE "%s/" YAP_RESET "\n", name);
  else
    printf(YAP_GREY "%s\n" YAP_RESET, name);
}

static void print_count(void) {
  printf("\nTotal Directories : " YAP_AQUA "%zu\n" YAP_RESET
         "Total Files       : " YAP_AQUA "%zu\n" YAP_RESET,
         dir_count, file_count);
}

static void list_dir_l(const char *path, i32 depth, b *last) {
  DIR *dir = opendir(path);

  if (!dir)
    return;

  char **entries = NULL;
  st count = 0;
  st capacity = 0;

  struct dirent *dp;

  while ((dp = readdir(dir)) != NULL) {
    const char *name = dp->d_name;

    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0 ||
        strcmp(name, ".git") == 0)
      continue;

    if (count == capacity) {
      st new_capacity = capacity ? capacity * 2 : 16;

      char **tmp = realloc(entries, new_capacity * sizeof(*entries));

      if (!tmp)
        break;

      entries = tmp;
      capacity = new_capacity;
    }

    entries[count] = strdup(name);

    if (!entries[count])
      break;

    count++;
  }

  closedir(dir);

  qsort(entries, count, sizeof(*entries), compare_entries);

  for (st i = 0; i < count; i++) {
    st len = strlen(path) + strlen(entries[i]) + 2;
    char sub_path[len];

    snprintf(sub_path, len, "%s/%s", path, entries[i]);

    b isdir = is_directory(sub_path);
    b is_last = (i == count - 1);

    print_entry(entries[i], isdir, depth, last, is_last);

    if (isdir) {
      dir_count++;

      last[depth] = is_last;
      list_dir_l(sub_path, depth + 1, last);
    } else {
      file_count++;
    }

    free(entries[i]);
  }

  free(entries);
}

void print_tree(const char *path) {
  file_count = 0;
  dir_count = 0;

  b last[256] = {false};

  printf("%s\n", path);
  list_dir_l(path, 0, last);

  print_count();
}
#endif /* if defi */
