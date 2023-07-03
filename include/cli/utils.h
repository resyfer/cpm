#ifndef __CPM_UTILS_H
#define __CPM_UTILS_H

#include <stdbool.h>
#include <stddef.h>

#include <global.h>

#define MIN(a, b)  a < b ? a : b

char *str_n_dup(const char *s, size_t max);
void str_rst(char *str);
bool str_ends_with(const char *string, const char *pattern);

FILE_TYPE cpm_get_path_type(const char path[PATH_LEN_MAX]);

#endif
