#include "log.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <global.h>
#include <build.h>
#include <unistd.h>
#include <utils.h>

static FILE_TYPE get_path_type(const char path[PATH_LEN_MAX]);

typedef struct {

	char file[PATH_LEN_MAX];
	char build_file[PATH_LEN_MAX]; // strlen(build_file) == 0 or, build_file[0] == '\0' for header files

	int dependents[SRC_NUM_MAX]; // Index of dependency in the array. TODO: Make sure the dependency does not refer to own index
	int n_dependents;

} dependency_node_t;

void process_c_preprocessor_output(dependency_node_t deps[SRC_NUM_MAX], int *n_deps, char path[PATH_LEN_MAX], char build_path[PATH_LEN_MAX], FILE* output) {

	char c;

	const int path_len = strlen(path);
	char temp_path[PATH_LEN_MAX] = {0};
	int idx_temp_path = 0;

	int space_count = 0;

	// Find dependent in deps array
	bool dependent_found = false;
	int dependent_found_idx = -1;
	for(int i = 0; i < *n_deps; i++) {
		if(!strcmp(deps[i].file, path)) {
			dependent_found = true;
			dependent_found_idx = i;
			break;
		}
	}

	if(!dependent_found) {
		strncpy(deps[*n_deps].file, path, PATH_LEN_MAX);
		dependent_found_idx = *n_deps;
		(*n_deps)++;
	}

	if(path_len >= 2 && !strcmp(path + (path_len - 2), ".c")) {
		printf("==> %s\n", deps[dependent_found_idx].file);
		strncpy(deps[dependent_found_idx].build_file, build_path, PATH_LEN_MAX);
		deps[dependent_found_idx].build_file[strlen(deps[dependent_found_idx].build_file) - 1] = 'o';
		printf("==> %s\n", deps[dependent_found_idx].build_file);
	}

	while(1) {

		c = fgetc(output);

		// Start parsing only from 2nd space
		if(space_count < 2) {
			if(c == ' ') space_count++;
			continue;
		}

		if(c == '\\' || c == '\n') {
			continue;
		}

		// File path finished, now inserting into deps
		if(c == ' ' || c == EOF) {

			// Find depdency in deps array
			bool dependency_found = false;
			int dependency_found_idx = -1;
			for(int i = 0; i < *n_deps; i++) {
				if(!strcmp(deps[i].file, temp_path)) {
					dependency_found = true;
					dependency_found_idx = i;
					break;
				}
			}

			if(!dependency_found) {
				strncpy(deps[*n_deps].file, temp_path, PATH_LEN_MAX);
				// Build file value taken care of later when it's a depdendent
				dependency_found_idx = *n_deps;
				(*n_deps)++;
			}

			deps[dependency_found_idx].dependents[deps[dependency_found_idx].n_dependents++] = dependent_found_idx;

			memset(&temp_path, 0, PATH_LEN_MAX);
			idx_temp_path = 0;


			if(c == EOF) {
				break;
			}

			continue;
		}

		if(idx_temp_path == PATH_LEN_MAX) {
			error("Path too long. Only %s could be parsed", temp_path);
			exit(1);
		}

		temp_path[idx_temp_path++] = c;

	}
}

void get_file_dependencies(dependency_node_t deps[SRC_NUM_MAX], int *n_deps, char path[PATH_LEN_MAX], char build_path[PATH_LEN_MAX], const char *include_str) {

	FILE_TYPE type = get_path_type(path);
	DIR *dir = NULL;
	struct dirent *directory_entry;
	char preprocessor_command[LONG_STR_MAX] = {0};
	int path_len = strlen(path), build_path_len = strlen(build_path);

	switch (type) {

	case REGULAR_FILE:

		snprintf(preprocessor_command, LONG_STR_MAX, "cpp -MM %s %s", include_str, path);

		FILE* output = popen(preprocessor_command, "r");

		process_c_preprocessor_output(deps, n_deps, path, build_path, output);

		pclose(output);

		break;

	case DIRECTORY:

		dir = opendir(path);
		if (!dir) {
			warning("Could not open directory: %s", path);
			break;
		}
		// Check if diectory exists in build

		while ((directory_entry = readdir(dir)) != NULL) {
			char new_path[PATH_LEN_MAX] = { 0 }, new_build_path[PATH_LEN_MAX] = { 0 };

			if (!strcmp(directory_entry->d_name, ".")
			    || !strcmp(directory_entry->d_name, "..")) {
				continue;
			}

			strncpy(new_path, path, PATH_LEN_MAX);
			strncpy(new_build_path, build_path, PATH_LEN_MAX);

			strncat(new_path, "/", PATH_LEN_MAX - path_len - 1);
			strncat(new_path, directory_entry->d_name,
				PATH_LEN_MAX - path_len - 2);
			strncat(new_build_path, "/", PATH_LEN_MAX - build_path_len - 1);
			strncat(new_build_path, directory_entry->d_name,
				PATH_LEN_MAX - build_path_len - 2);

			get_file_dependencies(deps, n_deps, new_path, new_build_path, include_str);
		}
		break;

	default:
		return;
	}

	if (dir) {
		closedir(dir);
		dir = NULL;
	}

}

void get_dependencies(dependency_node_t deps[SRC_NUM_MAX], int *n_deps) {

	// Setting header include string
	char *include_str = NULL;
	int include_str_size = 0;
	for(int i = 0; i < config.n_headeer_paths; i++) {
		int header_path_len = strlen(config.header_paths[i]);
		include_str = realloc(include_str, include_str_size + header_path_len + 6);

		sprintf(include_str + include_str_size, " -I./%s", config.header_paths[i]);
		include_str_size += header_path_len + 5;
		include_str[include_str_size] = 0;
	}

	char build_path[PATH_LEN_MAX] = {0};
	strncpy(build_path, "./" CPM_DIRECTORY "/build", PATH_LEN_MAX);

	for(int i = 0; i < config.n_src_paths; i++) {
		get_file_dependencies(deps, n_deps, config.src_paths[i], build_path, include_str);
	}

	if(include_str) {
		free(include_str);
	}
}

void
build()
{
	dependency_node_t deps[SRC_NUM_MAX];
	int n_deps = 0;
	memset(&deps, 0, SRC_NUM_MAX * sizeof(dependency_node_t));

	get_dependencies(deps, &n_deps);

	for(int i = 0; i < n_deps; i++) {

		logger("File:%s\n", deps[i].file);
		if(deps[i].build_file[0] != 0)
			logger("Build:%s\n", deps[i].build_file);

		for(int j = 0; j < deps[i].n_dependents; j++) {
			logger("    -%s\n", deps[deps[i].dependents[j]].file);
		}

	}

	// TODO: Eneter dependents in an array (unique entry for each) if a file is newer
	// TODO: than any of the build files of dependents.

}

static FILE_TYPE
get_path_type(const char path[PATH_LEN_MAX])
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