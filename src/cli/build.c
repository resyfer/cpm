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

typedef struct {

	char file[PATH_LEN_MAX];
	char build_file[PATH_LEN_MAX];	// strlen(build_file) == 0 or, build_file[0] == '\0' for header files

	int dependents[SRC_NUM_MAX];	// Index of dependency in the array. TODO: Make sure the dependency does not refer to own index
	int n_dependents;

} dependency_node_t;

static void get_dependencies(dependency_node_t deps[SRC_NUM_MAX], int *n_deps,
			     const char *include_str);
static void get_file_dependencies(dependency_node_t deps[SRC_NUM_MAX],
				  int *n_deps, const char path[PATH_LEN_MAX],
				  const char build_path[PATH_LEN_MAX],
				  const char *include_str);
static void set_header_str(char **include);
static void to_build_files(dependency_node_t deps[SRC_NUM_MAX], int n_deps,
			   bool build_idx_queue[SRC_NUM_MAX]);
static void add_to_build_queue(bool build_idx_queue[SRC_NUM_MAX],
			       const int deps_idx);

static void process_c_preprocessor_output(dependency_node_t deps[SRC_NUM_MAX],
					  int *n_deps,
					  const char path[PATH_LEN_MAX],
					  const char build_path[PATH_LEN_MAX],
					  FILE * output);
static void linker(dependency_node_t deps[SRC_NUM_MAX], int n_deps);

void
cpm_build()
{
	dependency_node_t deps[SRC_NUM_MAX];
	char *include_str = NULL;

	set_header_str(&include_str);

	int n_deps = 0;
	memset(&deps, 0, SRC_NUM_MAX * sizeof(dependency_node_t));

	get_dependencies(deps, &n_deps, include_str);

	bool build_idx_queue[SRC_NUM_MAX];
	memset(&build_idx_queue, 0, SRC_NUM_MAX * sizeof(bool));

	to_build_files(deps, n_deps, build_idx_queue);

	// Build
	cpm_logger("Building...\n");

	for (int i = 0; i < n_deps; i++) {
		if (!build_idx_queue[i]) {
			if (deps[i].build_file[0] != 0) {
				cpm_logger("File up-to-date: %s\n",
					   deps[i].file);
			}
			continue;
		} else {
			cpm_logger("Building file: %s\n", deps[i].file);
		}

		char build_str[LONG_STR_MAX] = { 0 };
		snprintf(build_str, LONG_STR_MAX - 1, "%s %s -c %s -o %s",
			 config.compiler, include_str, deps[i].file,
			 deps[i].build_file);

		if (system(build_str)) {
			cpm_error("Could not build: %s\n", deps[i].file);
			exit(1);
		}

	}

	//Linking
	linker(deps, n_deps);

	if (include_str) {
		free(include_str);
	}

	cpm_logger("Done! Build file written to: %s\n", config.output_file);
}

//----------------------------------

static void
set_header_str(char **include)
{

	cpm_logger("Generating include string...\n");

	char *include_str = NULL;
	int include_str_size = 0;

	for (int i = 0; i < config.n_headeer_paths; i++) {
		int header_path_len = strlen(config.header_paths[i]);
		include_str =
		    realloc(include_str,
			    include_str_size + header_path_len + 6);

		sprintf(include_str + include_str_size, " -I./%s",
			config.header_paths[i]);
		include_str_size += header_path_len + 5;
		include_str[include_str_size] = 0;
	}

	*include = include_str;
}

//----------------------------------

static void
get_dependencies(dependency_node_t deps[SRC_NUM_MAX], int *n_deps,
		 const char *include_str)
{
	char build_path[PATH_LEN_MAX] = { 0 };
	strncpy(build_path, "./" CPM_DIRECTORY "/build", PATH_LEN_MAX);

	for (int i = 0; i < config.n_src_paths; i++) {
		get_file_dependencies(deps, n_deps, config.src_paths[i],
				      build_path, include_str);
	}
}

static void
get_file_dependencies(dependency_node_t deps[SRC_NUM_MAX], int *n_deps,
		      const char path[PATH_LEN_MAX],
		      const char build_path[PATH_LEN_MAX],
		      const char *include_str)
{

	FILE_TYPE type = cpm_get_path_type(path);
	DIR *dir = NULL;
	struct dirent *directory_entry;
	char preprocessor_command[LONG_STR_MAX] = { 0 };
	int path_len = strlen(path), build_path_len = strlen(build_path);

	switch (type) {

	case REGULAR_FILE:

		snprintf(preprocessor_command, LONG_STR_MAX,
			 PREPROCESSOR " -MM %s %s", include_str, path);

		FILE *output = popen(preprocessor_command, "r");

		process_c_preprocessor_output(deps, n_deps, path, build_path,
					      output);

		pclose(output);

		break;

	case DIRECTORY:

		dir = opendir(path);
		if (!dir) {
			cpm_warning("Could not open directory: %s", path);
			break;
		}

		DIR *build_dir = opendir(build_path);
		if (build_dir) {
			closedir(build_dir);
			build_dir = NULL;
		} else {
			mkdir(build_path, 0777);
		}

		while ((directory_entry = readdir(dir)) != NULL) {
			char new_path[PATH_LEN_MAX] =
			    { 0 }, new_build_path[PATH_LEN_MAX] = { 0 };

			if (!strcmp(directory_entry->d_name, ".")
			    || !strcmp(directory_entry->d_name, "..")) {
				continue;
			}

			strncpy(new_path, path, PATH_LEN_MAX);
			strncpy(new_build_path, build_path, PATH_LEN_MAX);

			strncat(new_path, "/", PATH_LEN_MAX - path_len - 1);
			strncat(new_path, directory_entry->d_name,
				PATH_LEN_MAX - path_len - 2);
			strncat(new_build_path, "/",
				PATH_LEN_MAX - build_path_len - 1);
			strncat(new_build_path, directory_entry->d_name,
				PATH_LEN_MAX - build_path_len - 2);

			get_file_dependencies(deps, n_deps, new_path,
					      new_build_path, include_str);
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

static void
process_c_preprocessor_output(dependency_node_t deps[SRC_NUM_MAX], int *n_deps,
			      const char path[PATH_LEN_MAX],
			      const char build_path[PATH_LEN_MAX], FILE *output)
{

	char c;

	const int path_len = strlen(path);
	char temp_path[PATH_LEN_MAX] = { 0 };
	int idx_temp_path = 0;

	int space_count = 0;

	// Find dependent in deps array
	bool dependent_found = false;
	int dependent_found_idx = -1;
	for (int i = 0; i < *n_deps; i++) {
		if (!strcmp(deps[i].file, path)) {
			dependent_found = true;
			dependent_found_idx = i;
			break;
		}
	}

	if (!dependent_found) {
		strncpy(deps[*n_deps].file, path, PATH_LEN_MAX);
		dependent_found_idx = *n_deps;
		(*n_deps)++;
	}

	if (path_len >= 2 && !strcmp(path + (path_len - 2), ".c")) {
		strncpy(deps[dependent_found_idx].build_file, build_path,
			PATH_LEN_MAX);
		deps[dependent_found_idx].build_file[strlen
						     (deps
						      [dependent_found_idx].build_file)
						     - 1] = 'o';
	}

	while (1) {

		c = fgetc(output);

		// Start parsing only from 2nd space
		if (space_count < 2) {
			if (c == ' ')
				space_count++;
			continue;
		}

		if (c == '\\' || c == '\n') {
			continue;
		}
		// File path finished, now inserting into deps
		if (c == ' ' || c == EOF) {

			// Find depdency in deps array
			bool dependency_found = false;
			int dependency_found_idx = -1;
			for (int i = 0; i < *n_deps; i++) {
				if (!strcmp(deps[i].file, temp_path)) {
					dependency_found = true;
					dependency_found_idx = i;
					break;
				}
			}

			if (!dependency_found) {
				strncpy(deps[*n_deps].file, temp_path,
					PATH_LEN_MAX);
				// Build file value taken care of later when it's a depdendent
				dependency_found_idx = *n_deps;
				(*n_deps)++;
			}

			deps[dependency_found_idx].dependents[deps
							      [dependency_found_idx].
							      n_dependents++]
			    = dependent_found_idx;

			memset(&temp_path, 0, PATH_LEN_MAX);
			idx_temp_path = 0;

			if (c == EOF) {
				break;
			}

			continue;
		}

		if (idx_temp_path == PATH_LEN_MAX) {
			cpm_error("Path too long. Only %s could be parsed",
				  temp_path);
			exit(1);
		}

		temp_path[idx_temp_path++] = c;

	}
}

//----------------------------------

static void
to_build_files(dependency_node_t deps[SRC_NUM_MAX], int n_deps,
	       bool build_idx_queue[SRC_NUM_MAX])
{

	cpm_logger("Reteiving build files...\n");

	for (int i = 0; i < n_deps; i++) {

		if (deps[i].build_file[0] == 0) {
			// Build file

			// Build files have only source files as dependents.

			struct stat hdr_fstat;
			if (stat(deps[i].file, &hdr_fstat)) {
				cpm_error("Could not get stats for file: %s\n",
					  deps[i].file);
				exit(1);
			}

			for (int j = 0; j < deps[i].n_dependents; j++) {

				struct stat src_build_fstat;

				if (!stat
				    (deps[deps[i].dependents[j]].build_file,
				     &src_build_fstat)) {
					if ((hdr_fstat.st_ctim.tv_sec >
					     src_build_fstat.st_ctim.tv_sec)
					    || (hdr_fstat.st_ctim.tv_sec ==
						src_build_fstat.st_ctim.tv_sec
						&& hdr_fstat.st_ctim.tv_nsec >
						src_build_fstat.st_ctim.
						tv_nsec)) {
						/* Add to build queue */
					} else {
						/* No need to add to build queue */
						continue;
					}
				}

				add_to_build_queue(build_idx_queue,
						   deps[i].dependents[j]);
			}

		} else {
			// Source file

			// Source files have only themselved as dependents.

			struct stat src_fstat, build_fstat;

			if (!stat(deps[i].file, &src_fstat)) {
				if (!stat(deps[i].build_file, &build_fstat)) {
					if ((src_fstat.st_ctim.tv_sec >
					     build_fstat.st_ctim.tv_sec)
					    || (src_fstat.st_ctim.tv_sec ==
						build_fstat.st_ctim.tv_sec
						&& src_fstat.st_ctim.tv_nsec >
						build_fstat.st_ctim.tv_nsec)) {
						/* Add to build queue */
					} else {
						/* No need to add to build queue */
						continue;
					}
				}
			} else {
				cpm_error("Could not get stats for file: %s\n",
					  deps[i].file);
				exit(1);
			}

			add_to_build_queue(build_idx_queue, i);

		}

	}
}

static void
add_to_build_queue(bool build_idx_queue[SRC_NUM_MAX], const int deps_idx)
{
	build_idx_queue[deps_idx] = true;
}

//----------------------------------

static void
linker(dependency_node_t deps[SRC_NUM_MAX], int n_deps)
{

	char linker_str[LONG_STR_MAX] = { 0 };
	char *linker_str_idx = NULL;

	cpm_logger("Linking...\n");

	linker_str_idx = linker_str;
	linker_str_idx += snprintf(linker_str_idx, PATH_LEN_MAX, LINKER);	// Adding linker command

	for (int i = 0; i < n_deps; i++) {
		linker_str_idx += snprintf(linker_str_idx, PATH_LEN_MAX, " %s", deps[i].build_file);	// Adding object files
	}

	linker_str_idx += snprintf(linker_str_idx, PATH_LEN_MAX, " -o %s", config.output_file);	// Specifying output

	if (system(linker_str)) {
		cpm_error("Could not link: %s\n", config.output_file);
		exit(1);
	}

}
