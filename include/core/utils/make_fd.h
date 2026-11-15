#pragma once

#include <stdbool.h>

bool create_dir(const char *dir);
bool create_file(const char *path);
bool write_file(const char *path, const char *content);
