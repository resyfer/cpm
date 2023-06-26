#include "version.h"
#include <stdio.h>

#include <help.h>

void
help()
{
	version();
	printf("  A C Package Manager\n");
	printf("\n");
	printf("Usage:\n");
	printf("  %s [ COMMAND ]\n", PROJECT);
	printf("\n");
	printf("Options:\n");
	printf("  -h, --help     Display this helpful text and exit\n");
	printf("  -v, --version  Display version information and exit\n");
	printf
	    ("  -x, --init     Initialize a project and related directories\n");
	printf("  -b, --build    Build the project\n");
	printf("\n");
	printf("%s: %s\n", PROJECT, PROJECT_REPO);
	printf("Report bugs: %s\n", PROJECT_ISSUES);
}
