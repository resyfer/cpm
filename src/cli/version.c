#include <stdio.h>

#include <version.h>

void
cpm_version()
{
	/* *INDENT-OFF* */
	printf("%s %s\n", PROJECT, VERSION);
	/* *INDENT-ON* */
}
