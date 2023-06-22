#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <log.h>

static void print_log(const char *prefix, const char *format, va_list args);

void
warning(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	print_log("WARNING", format, args);

	va_end(args);
}

void
error(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	print_log("ERROR", format, args);

	va_end(args);
}

static void
print_log(const char *prefix, const char *format, va_list args)
{
	char *new_format =
	    malloc(sizeof(char) * (strlen(prefix) + strlen(format) + 4));
	sprintf(new_format, "[%s] %s", prefix, format);

	vprintf(new_format, args);

	free(new_format);

	new_format = NULL;
}
