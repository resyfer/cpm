#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <init.h>
#include <global.h>

static bool init_file_exists();

void
init()
{
	if (init_file_exists()) {
		warning("Init file already exists\n");
		exit(1);
	}

	FILE *init_file;

	init_file = fopen(".cpm", "w");

	fprintf(init_file, "[global]\n");
	fprintf(init_file, "policy = %s\n", POLICY);
	fprintf(init_file, "type = bin\n");
	fprintf(init_file, "headers = include\n");
	fprintf(init_file, "src = src\n");
	fprintf(init_file, "out = project\n");
	fprintf(init_file, "\n");
	fprintf(init_file, "[dependencies]\n");
	fprintf(init_file, "\n");
	fprintf(init_file, "[versions]\n");
	fprintf(init_file, "\n");

	fclose(init_file);
	init_file = NULL;
}

static bool
init_file_exists()
{
	FILE *init_file;
	bool is_init_file_exist = false;

	init_file = fopen(".cpm", "r");

	if (init_file) {
		is_init_file_exist = true;
		fclose(init_file);
		init_file = NULL;
	}

	return is_init_file_exist;
}
