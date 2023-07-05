#ifndef __CPM_ARGS_H
#define __CPM_ARGS_H

#include <stdbool.h>

typedef struct {
	bool help;
	bool version;
	bool init;
	bool global;
	bool yes;
	char **add;
	int n_add;
	char **find;
	int n_find;
	bool ld;
	char **remove;
	int n_remove;
	bool clean;
	bool build;
	bool run;
	char **update;
	int n_update;
} args_t;

extern args_t arguments;

void cpm_parse_cli_args(int argc, char **argv);

#endif
