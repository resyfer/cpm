#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <init.h>
#include <global.h>
#include <sys/stat.h>

static bool init_file_exists();
static void create_cpm_modules();

void
init()
{
	if (init_file_exists()) {
		warning("Init file already exists\n");
		exit(1);
	}

	create_cpm_modules();

	FILE *init_file;

	init_file = fopen(CONFIG_FILE_NAME, "w");

	fprintf(init_file, "[global]\n");
	fprintf(init_file, "policy = %s\n", POLICY);
	fprintf(init_file, "type = bin\n");
	fprintf(init_file, "compiler = gcc\n");
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

	logger("Project initialized\n");
}

static bool
init_file_exists()
{
	FILE *init_file;
	bool is_init_file_exist = false;

	init_file = fopen(CONFIG_FILE_NAME, "r");

	if (init_file) {
		is_init_file_exist = true;
		fclose(init_file);
		init_file = NULL;
	}

	return is_init_file_exist;
}

static void
create_cpm_modules()
{
	int result = mkdir("./" CPM_DIRECTORY, S_IRWXU | S_IRWXG | S_IRWXO);
	if (result) {
		error("Could not create `" CPM_DIRECTORY "` directory\n");
		exit(1);
	}

	result = mkdir("./" CPM_DIRECTORY "/lib", S_IRWXU | S_IRWXG | S_IRWXO);
	if (result) {
		error("Could not create `" CPM_DIRECTORY "/lib` directory\n");
		exit(1);
	}

	result =
	    mkdir("./" CPM_DIRECTORY "/.cache", S_IRWXU | S_IRWXG | S_IRWXO);
	if (result) {
		error("Could not create `" CPM_DIRECTORY
		      "/.cache` directory\n");
		exit(1);
	}

	result =
	    mkdir("./" CPM_DIRECTORY "/build", S_IRWXU | S_IRWXG | S_IRWXO);
	if (result) {
		error("Could not create `" CPM_DIRECTORY "/build` directory\n");
		exit(1);
	}

	result =
	    mkdir("./" CPM_DIRECTORY "/include", S_IRWXU | S_IRWXG | S_IRWXO);
	if (result) {
		error("Could not create `" CPM_DIRECTORY
		      "/include` directory\n");
		exit(1);
	}
	// Add to .gitignore
	system("echo \"\n" CONFIG_FILE_NAME "\n" CPM_DIRECTORY
	       "\" >> .gitignore");
}
