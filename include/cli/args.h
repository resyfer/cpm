#ifndef __ARGS_H
#define __ARGS_H

#include <stdbool.h>

typedef struct args {
  bool help;
  bool version;
  bool init;
  bool global;
  bool yes;
  char** add;
  int n_add;
  char** find;
  int n_find;
  bool ld;
  char** remove;
  int n_remove;
  bool clean;
  bool build;
  bool run;
  char** update;
  int n_update;
} args_t;

extern args_t arguments;

void parse_cli_args(int argc, char** argv);

#endif