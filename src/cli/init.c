#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>

#include <init.h>
#include <global.h>
#include <utils.h>

static bool init_file_exists();
static void create_cpm_modules();
static void create_global_store();

void
cpm_init()
{
	if (init_file_exists()) {
		cpm_warning("Init file already exists\n");
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

	cpm_logger("Project initialized\n");
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
create_global_store()
{
	int result = 0;

	// Check
	cpm_logger("Checking global store...\n");

	DIR *global_store_dir = opendir(CPM_GLOBAL_STORE);
	if (global_store_dir) {
		cpm_logger
		    ("Global store already exists. Skipping creation...\n");
		closedir(global_store_dir);
		return;
	}
	// Create
	cpm_logger("Global store not found...\n");
	cpm_logger("Creating global store...\n");

	char *buf = NULL;

	result = mkdir(CPM_GLOBAL_STORE, S_IRWXU | S_IRWXG | S_IRWXO);
	if (result) {
		cpm_error("Could not create `%s` directory\n",
			  CPM_GLOBAL_STORE);
		exit(1);
	}

	buf = cpm_str_heap_cat(CPM_GLOBAL_STORE, "/lib");
	result = mkdir(buf, S_IRWXU | S_IRWXG | S_IRWXO);
	if (result) {
		cpm_error("Could not create `%s/lib` directory\n",
			  CPM_GLOBAL_STORE);
		exit(1);
	}
	free(buf);
	buf = NULL;

	buf = cpm_str_heap_cat(CPM_GLOBAL_STORE, "/bin");
	result = mkdir(buf, S_IRWXU | S_IRWXG | S_IRWXO);
	if (result) {
		cpm_error("Could not create `%s/bin` directory\n",
			  CPM_GLOBAL_STORE);
		exit(1);
	}
	free(buf);
	buf = NULL;

	buf = cpm_str_heap_cat(CPM_GLOBAL_STORE, "/repos");
	result = mkdir(buf, S_IRWXU | S_IRWXG | S_IRWXO);
	if (result) {
		cpm_error("Could not create `%s/repos` directory\n",
			  CPM_GLOBAL_STORE);
		exit(1);
	}
	free(buf);
	buf = NULL;
}

static void
create_cpm_modules()
{
	create_global_store();

	cpm_logger("Initializing local project...\n");

	// Project Local Folder
	int result = mkdir("./" CPM_DIRECTORY, S_IRWXU | S_IRWXG | S_IRWXO);
	if (result) {
		cpm_error("Could not create `" CPM_DIRECTORY "` directory\n");
		exit(1);
	}

	result = mkdir("./" CPM_DIRECTORY "/lib", S_IRWXU | S_IRWXG | S_IRWXO);
	if (result) {
		cpm_error("Could not create `" CPM_DIRECTORY
			  "/lib` directory\n");
		exit(1);
	}

	result =
	    mkdir("./" CPM_DIRECTORY "/build", S_IRWXU | S_IRWXG | S_IRWXO);
	if (result) {
		cpm_error("Could not create `" CPM_DIRECTORY
			  "/build` directory\n");
		exit(1);
	}

	result =
	    mkdir("./" CPM_DIRECTORY "/include", S_IRWXU | S_IRWXG | S_IRWXO);
	if (result) {
		cpm_error("Could not create `" CPM_DIRECTORY
			  "/include` directory\n");
		exit(1);
	}
	// Add to .gitignore
	system("echo \"\n" CONFIG_FILE_NAME "\n" CPM_DIRECTORY
	       "\" >> .gitignore");
}
