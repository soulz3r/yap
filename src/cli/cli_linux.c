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

static b is_directory(const c *path) {
  struct stat info;
  return stat(path, &info) == 0 && S_ISDIR(info.st_mode);
}

static i32 compare_entries(const v *a, const v *b) {
  const c *const *x = a;
  const c *const *y = b;

  return strcmp(*x, *y);
}

static v print_entry(const c *name, b isdir, i32 depth,
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

static v print_count(v) {
  printf("\nTotal Directories : " YAP_AQUA "%zu\n" YAP_RESET
         "Total Files       : " YAP_AQUA "%zu\n" YAP_RESET,
         dir_count, file_count);
}

static v list_dir_l(const c *path, i32 depth, b *last) {
  DIR *dir = opendir(path);

  if (!dir)
    return;

  c **entries = NULL;
  st count = 0;
  st capacity = 0;

  struct dirent *dp;

  while ((dp = readdir(dir)) != NULL) {
    const c *name = dp->d_name;

    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0 ||
        strcmp(name, ".git") == 0)
      continue;

    if (count == capacity) {
      st new_capacity = capacity ? capacity * 2 : 16;

      c **tmp = realloc(entries, new_capacity * sizeof(*entries));

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
    c sub_path[len];

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

v print_tree(const c *path) {
  file_count = 0;
  dir_count = 0;

  b last[256] = {false};

  printf("%s\n", path);
  list_dir_l(path, 0, last);

  print_count();
}
#endif /* if defi */
