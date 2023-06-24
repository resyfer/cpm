#include <stdio.h>
#include <stdlib.h>

#include <global.h>
#include <build.h>

void build() {

	char build_str[STR_MAX] = {0};
	char *idx = build_str;

	idx += snprintf(idx, STR_MAX - (idx - build_str) - 1, "%s -I./cpm_modules/include -L./cpm_modules/lib", config.compiler);

	for(int i = 0; i < config.n_src_paths; i++) {
		idx += snprintf(idx, STR_MAX - (idx - build_str) - 1, " %s/*.c", config.src_paths[i]); // TODO: Recursive
	}

	for(int i = 0; i < config.n_headeer_paths; i++) {
		idx += snprintf(idx, STR_MAX - (idx - build_str) - 1, " -I%s", config.header_paths[i]);
	}

	idx += snprintf(idx, STR_MAX - (idx - build_str) - 1, " -o %s", config.output_file);

	system(build_str); // TODO: Deal with return value

}