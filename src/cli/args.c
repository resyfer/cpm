#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include <global.h>
#include <args.h>

/*

CLI:
$ cpm --help, cpm -?
$ cpm --version, cpm -v
$ cpm --init, cpm -x
$ cpm --add <pkg>:<vers> ..., cpm -a <pkg>:<ver> ...
$ cpm --find <pkg>:<ver> ..., cpm -f <pkg>:<ver> ...
$ cpm --ld <pkg>:<ver> ..., cpm -l <pkg>:<ver> ...
$ cpm --remove <pkg> ..., cpm -r <pkg> ...
$ cpm --clean, cpm -c // Cleans out any package binaries that are no longer dependent on
$ cpm --update <pkg> ... // Updates according to version format mention in cpm.build
$ cpm --global --add <pkg>:<vers> ..., cpm -g -a <pkg>:<ver> ...
$ cpm build
$ cpm run

*/

args_t arguments;

void
parse_cli_args(int argc, char** argv)
{
	int c, option_index;
	struct option long_options[] =
	{
		{"version", no_argument, NULL, 'v'},
		{"help", no_argument, NULL, 'h'},
		{"init", no_argument, NULL, 'x'},
		{"global", no_argument, NULL, 'g'},
		{"yes", no_argument, NULL, 'y'},
		{"add", required_argument, NULL, 'a'},
		{"find", required_argument, NULL, 'f'},
		{"ld", required_argument, NULL, 'l'},
		{"remove", required_argument, NULL, 'd'},
		{"clean", no_argument, NULL, 'c'},
		{"build", no_argument, NULL, 'b'},
		{"run", no_argument, NULL, 'r'},
		{"update", required_argument, NULL, 'u'},
		{0, 0, 0, 0}
	};

	memset(&arguments, 0, sizeof(args_t));

	while((c = getopt_long(argc, argv, "vhxgya:f:l:r:s:cbru:", long_options, &option_index)) != -1) {

		if(c == -1) {
			break;
		}

		switch(c) {
			case 'v':
				arguments.version = true;
				break;

			case 'h':
				arguments.help = true;
				break;

			case 'x':
				arguments.init = true;
				break;

			case 'g':
				arguments.global = true;
				break;

			case 'y':
				arguments.yes = true;
				break;

			case 'a':
				optind--;
				while(optind < argc && argv[optind][0] != '-') {
					arguments.n_add++;
					arguments.add = realloc(arguments.add,
											arguments.n_add * sizeof(char*));
					arguments.add[arguments.n_add - 1] = argv[optind];
					optind++;
				}
				if(arguments.n_add) {
					optind--;
				} else {
					arguments.help = true;
					goto exit_args;
				}
				break;

			case 'f':
				optind--;
				while(optind < argc && argv[optind][0] != '-') {
					arguments.n_find++;
					arguments.find = realloc(arguments.find,
											arguments.n_find * sizeof(char*));
					arguments.find[arguments.n_find - 1] = argv[optind];
					optind++;
				}
				if(arguments.n_find) {
					optind--;
				} else {
					arguments.help = true;
					goto exit_args;
				}
				break;

			case 'l':
				arguments.ld = true;
				break;

			case 'd':
				optind--;
				while(optind < argc && argv[optind][0] != '-') {
					arguments.n_remove++;
					arguments.remove = realloc(arguments.remove,
											arguments.n_remove * sizeof(char*));
					arguments.remove[arguments.n_remove - 1] = argv[optind];
					optind++;
				}
				if(arguments.n_remove) {
					optind--;
				} else {
					arguments.help = true;
					goto exit_args;
				}
				break;

			case 'c':
				arguments.clean = true;
				break;

			case 'b':
				arguments.build = true;
				break;

			case 'r':
				arguments.run = true;
				break;

			case 'u':
				optind--;
				while(optind < argc && argv[optind][0] != '-') {
					arguments.n_update++;
					arguments.update = realloc(arguments.update,
											arguments.n_update * sizeof(char*));
					arguments.update[arguments.n_update - 1] = argv[optind];
					optind++;
				}
				if(arguments.n_update) {
					optind--;
				} else {
					arguments.help = true;
					goto exit_args;
				}
				break;

			default:
				printf("Option %c required argument\n", optopt);
				arguments.help = true;
				goto exit_args;

		}
	}

exit_args:
	; // Empty statement to keep linters happy about the goto at the end of function
}