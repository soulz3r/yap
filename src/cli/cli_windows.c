#if defined(YAP_PLATFORM_WINDOWS)

#include "defines.h"

#include "cli/colors.h"
#include "cli/tree.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

typedef struct tree_entry {
  c *name;
  b is_directory;
  b is_reparse_point;
} TREE_ENTRY;

static st file_count = 0;
static st dir_count = 0;

static c *copy_string(const c *source) {
  st length = strlen(source) + 1;
  c *copy = malloc(length);

  if (copy != NULL) {
    memcpy(copy, source, length);
  }

  return copy;
}

static i32 compare_entries(const v *a, const v *b) {
  const TREE_ENTRY *x = a;
  const TREE_ENTRY *y = b;

  i32 result = _stricmp(x->name, y->name);
  return result != 0 ? result : strcmp(x->name, y->name);
}

static v free_entries(TREE_ENTRY *entries, st count) {
  for (st i = 0; i < count; i++) {
    free(entries[i].name);
  }

  free(entries);
}

static c *make_search_path(const c *path) {
  st length = strlen(path);
  b has_separator =
      length > 0 && (path[length - 1] == '\\' || path[length - 1] == '/');

  if (length > SIZE_MAX - 3) {
    return NULL;
  }

  st capacity = length + (has_separator ? 2 : 3);
  c *search_path = malloc(capacity);

  if (search_path == NULL) {
    return NULL;
  }

  snprintf(search_path, capacity, "%s%s*", path, has_separator ? "" : "\\");
  return search_path;
}

static c *make_child_path(const c *path, const c *name) {
  st path_length = strlen(path);
  st name_length = strlen(name);
  b has_separator =
      path_length > 0 &&
      (path[path_length - 1] == '\\' || path[path_length - 1] == '/');

  if (path_length > SIZE_MAX - name_length - 2) {
    return NULL;
  }

  st capacity = path_length + name_length + (has_separator ? 1 : 2);
  c *child_path = malloc(capacity);

  if (child_path == NULL) {
    return NULL;
  }

  snprintf(child_path, capacity, "%s%s%s", path,
           has_separator ? "" : "\\", name);
  return child_path;
}

static b read_entries(const c *path, TREE_ENTRY **entries_out,
                         st *count_out) {
  c *search_path = make_search_path(path);

  if (search_path == NULL) {
    fprintf(stderr, "yap: unable to allocate directory search path\n");
    return false;
  }

  WIN32_FIND_DATAA data;
  HANDLE find_handle = FindFirstFileA(search_path, &data);
  free(search_path);

  if (find_handle == INVALID_HANDLE_VALUE) {
    fprintf(stderr, "yap: unable to read directory '%s' (Windows error %lu)\n",
            path, (unsigned long)GetLastError());
    return false;
  }

  TREE_ENTRY *entries = NULL;
  st count = 0;
  st capacity = 0;
  b success = true;

  do {
    const c *name = data.cFileName;

    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0 ||
        strcmp(name, ".git") == 0) {
      continue;
    }

    if (count == capacity) {
      st new_capacity = capacity == 0 ? 16 : capacity * 2;

      if (new_capacity < capacity ||
          new_capacity > SIZE_MAX / sizeof(*entries)) {
        success = false;
        break;
      }

      TREE_ENTRY *new_entries =
          realloc(entries, new_capacity * sizeof(*entries));

      if (new_entries == NULL) {
        success = false;
        break;
      }

      entries = new_entries;
      capacity = new_capacity;
    }

    c *entry_name = copy_string(name);

    if (entry_name == NULL) {
      success = false;
      break;
    }

    entries[count].name = entry_name;
    entries[count].is_directory =
        (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    entries[count].is_reparse_point =
        (data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0;
    count++;
  } while (FindNextFileA(find_handle, &data));

  DWORD find_error = GetLastError();
  FindClose(find_handle);

  if (find_error != ERROR_NO_MORE_FILES) {
    success = false;
  }

  if (!success) {
    fprintf(stderr, "yap: failed while reading directory '%s'\n", path);
    free_entries(entries, count);
    return false;
  }

  if (count > 1) {
    qsort(entries, count, sizeof(*entries), compare_entries);
  }

  *entries_out = entries;
  *count_out = count;
  return true;
}

static v print_entry(const c *name, b is_directory, i32 depth,
                        const b *last, b is_last) {
  for (i32 i = 0; i < depth; i++) {
    printf(last[i] ? "    " : YAP_DIM "│   " YAP_RESET);
  }

  printf(is_last ? YAP_DIM "└── " YAP_RESET : YAP_DIM "├── " YAP_RESET);

  if (is_directory) {
    printf(YAP_BLUE "%s/" YAP_RESET "\n", name);
  } else {
    printf(YAP_GREY "%s\n" YAP_RESET, name);
  }
}

static v list_directory(const c *path, i32 depth, b *last) {
  TREE_ENTRY *entries = NULL;
  st count = 0;

  if (!read_entries(path, &entries, &count)) {
    return;
  }

  for (st i = 0; i < count; i++) {
    b is_last = i == count - 1;
    TREE_ENTRY *entry = &entries[i];

    print_entry(entry->name, entry->is_directory, depth, last, is_last);

    if (entry->is_directory) {
      dir_count++;

      // Do not follow junctions or symbolic links; they can form cycles.
      if (!entry->is_reparse_point && depth < 255) {
        c *child_path = make_child_path(path, entry->name);

        if (child_path == NULL) {
          fprintf(stderr, "yap: unable to allocate path for '%s'\n",
                  entry->name);
        } else {
          last[depth] = is_last;
          list_directory(child_path, depth + 1, last);
          free(child_path);
        }
      }
    } else {
      file_count++;
    }
  }

  free_entries(entries, count);
}

static v print_count(v) {
  printf("\nTotal Directories : " YAP_AQUA "%zu\n" YAP_RESET
         "Total Files       : " YAP_AQUA "%zu\n" YAP_RESET,
         dir_count, file_count);
}

static v print_tree_utf8(const c *path) {
  if (path == NULL || path[0] == '\0') {
    fprintf(stderr, "yap: tree requires a valid directory path\n");
    return;
  }

  DWORD attributes = GetFileAttributesA(path);

  if (attributes == INVALID_FILE_ATTRIBUTES) {
    fprintf(stderr, "yap: cannot access directory '%s' "
                    "(Windows error %lu)\n",
            path, (unsigned long)GetLastError());
    return;
  }

  if ((attributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
    fprintf(stderr, "yap: '%s' is not a directory\n", path);
    return;
  }

  file_count = 0;
  dir_count = 0;

  b last[256] = {false};

  printf("%s\n", path);
  list_directory(path, 0, last);
  print_count();
}

v print_tree(const c *path) {
  const UINT previous_code_page = GetConsoleOutputCP();
  const b restore_code_page =
      previous_code_page != 0 && SetConsoleOutputCP(CP_UTF8) != 0;

  print_tree_utf8(path);
  fflush(stdout);

  if (restore_code_page) {
    SetConsoleOutputCP(previous_code_page);
  }
}

#endif
