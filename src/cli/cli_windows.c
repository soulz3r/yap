#if defined(YAP_PLATFORM_WINDOWS)

#include "cli/colors.h"
#include "cli/tree.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

typedef struct tree_entry {
  char *name;
  bool is_directory;
  bool is_reparse_point;
} TREE_ENTRY;

static size_t file_count = 0;
static size_t dir_count = 0;

static char *copy_string(const char *source) {
  size_t length = strlen(source) + 1;
  char *copy = malloc(length);

  if (copy != NULL) {
    memcpy(copy, source, length);
  }

  return copy;
}

static int compare_entries(const void *a, const void *b) {
  const TREE_ENTRY *x = a;
  const TREE_ENTRY *y = b;

  int result = _stricmp(x->name, y->name);
  return result != 0 ? result : strcmp(x->name, y->name);
}

static void free_entries(TREE_ENTRY *entries, size_t count) {
  for (size_t i = 0; i < count; i++) {
    free(entries[i].name);
  }

  free(entries);
}

static char *make_search_path(const char *path) {
  size_t length = strlen(path);
  bool has_separator =
      length > 0 && (path[length - 1] == '\\' || path[length - 1] == '/');

  if (length > SIZE_MAX - 3) {
    return NULL;
  }

  size_t capacity = length + (has_separator ? 2 : 3);
  char *search_path = malloc(capacity);

  if (search_path == NULL) {
    return NULL;
  }

  snprintf(search_path, capacity, "%s%s*", path, has_separator ? "" : "\\");
  return search_path;
}

static char *make_child_path(const char *path, const char *name) {
  size_t path_length = strlen(path);
  size_t name_length = strlen(name);
  bool has_separator =
      path_length > 0 &&
      (path[path_length - 1] == '\\' || path[path_length - 1] == '/');

  if (path_length > SIZE_MAX - name_length - 2) {
    return NULL;
  }

  size_t capacity = path_length + name_length + (has_separator ? 1 : 2);
  char *child_path = malloc(capacity);

  if (child_path == NULL) {
    return NULL;
  }

  snprintf(child_path, capacity, "%s%s%s", path,
           has_separator ? "" : "\\", name);
  return child_path;
}

static bool read_entries(const char *path, TREE_ENTRY **entries_out,
                         size_t *count_out) {
  char *search_path = make_search_path(path);

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
  size_t count = 0;
  size_t capacity = 0;
  bool success = true;

  do {
    const char *name = data.cFileName;

    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0 ||
        strcmp(name, ".git") == 0) {
      continue;
    }

    if (count == capacity) {
      size_t new_capacity = capacity == 0 ? 16 : capacity * 2;

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

    char *entry_name = copy_string(name);

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

static void print_entry(const char *name, bool is_directory, int depth,
                        const bool *last, bool is_last) {
  for (int i = 0; i < depth; i++) {
    printf(last[i] ? "    " : YAP_DIM "│   " YAP_RESET);
  }

  printf(is_last ? YAP_DIM "└── " YAP_RESET : YAP_DIM "├── " YAP_RESET);

  if (is_directory) {
    printf(YAP_BLUE "%s/" YAP_RESET "\n", name);
  } else {
    printf(YAP_GREY "%s\n" YAP_RESET, name);
  }
}

static void list_directory(const char *path, int depth, bool *last) {
  TREE_ENTRY *entries = NULL;
  size_t count = 0;

  if (!read_entries(path, &entries, &count)) {
    return;
  }

  for (size_t i = 0; i < count; i++) {
    bool is_last = i == count - 1;
    TREE_ENTRY *entry = &entries[i];

    print_entry(entry->name, entry->is_directory, depth, last, is_last);

    if (entry->is_directory) {
      dir_count++;

      // Do not follow junctions or symbolic links; they can form cycles.
      if (!entry->is_reparse_point && depth < 255) {
        char *child_path = make_child_path(path, entry->name);

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

static void print_count(void) {
  printf("\nTotal Directories : " YAP_AQUA "%zu\n" YAP_RESET
         "Total Files       : " YAP_AQUA "%zu\n" YAP_RESET,
         dir_count, file_count);
}

void print_tree(const char *path) {
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

  bool last[256] = {false};

  printf("%s\n", path);
  list_directory(path, 0, last);
  print_count();
}

#endif
