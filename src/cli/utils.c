#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <global.h>
#include <utils.h>

char *
cpm_str_n_dup(const char *s, size_t max)
{
	size_t s_len = strnlen(s, STR_MAX - 1);
	char *s_dup = calloc(s_len + 1, sizeof(char));
	memcpy(s_dup, s, s_len);
	return s_dup;
}

void
cpm_str_rst(char *str)
{
	int i = 0;
	while (str[i])
		str[i++] = 0;
}

bool
cpm_str_ends_with(const char *string, const char *pattern)
{
	int str_len = strlen(string), pat_len = strlen(pattern);
	return !strncmp(string + (str_len - pat_len), pattern,
			MIN(str_len, pat_len));
}

FILE_TYPE
cpm_get_path_type(const char path[PATH_LEN_MAX])
{
	struct stat fstat;
	stat(path, &fstat);

	if (S_ISREG(fstat.st_mode)) {
		return REGULAR_FILE;
	} else if (S_ISDIR(fstat.st_mode)) {
		return DIRECTORY;
	} else {
		return UNKNOWN;
	}
}
