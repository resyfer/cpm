#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#ifdef __unix__
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <global.h>
#include <parser.h>
#include <utils.h>

parse_t parse;

void
cpm_parser()
{
	memset(&parse, 0, sizeof(parse_t));

	/* Opening config file and creating a memory map to its contents */

#ifdef __unix__

	int conf_fd = open(CONFIG_FILE_NAME, O_RDONLY);
	if (conf_fd == -1) {
		cpm_error("Config File Open Error\n");
		exit(1);
	}

	struct stat s;

	if (fstat(conf_fd, &s) != 0) {
		cpm_error("Config Does Not Exist\n");
		exit(1);
	}

	size_t conf_size = s.st_size;

	char *file = mmap(NULL, conf_size, PROT_READ, MAP_PRIVATE, conf_fd, 0);
	if (file == MAP_FAILED) {
		cpm_error("MMap Error\n");
		close(conf_fd);
		exit(1);
	}
#elif _WIN32

	// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
	HANDLE h_file = CreateFileA(path, GENERIC_READ,
				    FILE_SHARE_READ, NULL, OPEN_ALWAYS,
				    FILE_ATTRIBUTE_NORMAL,
				    NULL);

	if (h_file == INVALID_HANDLE_VALUE) {
		error("Failed to open config file\n");
		goto end;
	}

	HANDLE h_map_file =
	    CreateFileMapping(h_file, NULL, PAGE_READONLY, 0, 0, NULL);
	if (!h_map_file) {
		error("File Map Error\n");
		goto end;
	}

	char *file = MapViewOfFile(h_map_file, FILE_MAP_READ, 0, 0, 0);
	if (!file) {
		error("Map View Error\n");
		goto end;
	}

	DWORD conf_size = GetFileSize(h_file, NULL);

#endif

	/* Parsing the file contents and gathering the various tables, rows, etc. */

	// Takes O(n) time to parse through the config, not counting operations
	// to copy to and from the string buffer.

	char buf[STR_MAX];
	int buf_idx = 0;
	bool is_invalid = false;

	// Initialize Config
	parse.n_tables = 0;
	parse.tables = NULL;

	for (int i = 0; i < conf_size; i++) {
		// {LF}*
		while ((i < conf_size && file[i] == '\n')
		       || (i < conf_size - 1 && file[i] == '\r'
			   && file[i + 1] == '\n')) {
			if (file[i] == '\n')
				i++;	// LF
			else
				i += 2;	// CRLF
		}

		// EOF
		if (i == conf_size) {
			goto end;
		}
		// TABLE
		if (file[i] == '[') {

			// HEADER
			table_t *table = malloc(sizeof(table_t));
			table->n_rows = 0;
			table->rows = NULL;
			table->header = NULL;

			cpm_str_rst(buf);
			buf_idx = 0;
			i++;
			while (buf_idx < STR_MAX && i < conf_size
			       && file[i] != ']')
				buf[buf_idx++] = file[i++];

			if (i == conf_size || buf_idx == STR_MAX
			    || file[i] != ']') {
				is_invalid = true;
				goto invalid;
			}
			i++;

			table->header = cpm_str_n_dup(buf, STR_MAX);

			// COLLECTION
			while (true) {
				// {LF}+
				while ((i < conf_size && file[i] == '\n')
				       || (i < conf_size - 1 && file[i] == '\r'
					   && file[i + 1] == '\n')) {
					if (file[i] == '\n')
						i++;	// LF
					else
						i += 2;	// CRLF
				}

				// EOF
				if (i == conf_size) {
					break;
				}
				// TABLE
				if (file[i] == '[') {
					i--;	// For next iteration of outermost for loop
					break;
				}

				row_t *row = malloc(sizeof(row_t));

				// TEXT (Key)
				cpm_str_rst(buf);
				buf_idx = 0;
				while (buf_idx < STR_MAX && i < conf_size
				       && (file[i] != ' ' && file[i] != '='))
					buf[buf_idx++] = file[i++];

				if (i == conf_size || buf_idx == STR_MAX) {
					is_invalid = true;
					goto invalid;
				}

				row->key = cpm_str_n_dup(buf, STR_MAX);

				// {WS}*
				while (i < conf_size && file[i] == ' ')
					i++;

				// EQUAL
				if (buf_idx < STR_MAX && i < conf_size
				    && file[i] == '=') {
					i++;
				} else {
					is_invalid = true;
					goto invalid;
				}

				// {WS}*
				while (i < conf_size && file[i] == ' ')
					i++;

				if (i == conf_size || buf_idx == STR_MAX) {
					is_invalid = true;
					goto invalid;
				}
				// TEXT (Value)
				cpm_str_rst(buf);
				buf_idx = 0;
				while (buf_idx < STR_MAX && i < conf_size
				       && !(file[i] == '\n'
					    || (file[i] == '\r'
						&& file[i + 1] == '\n')))
					buf[buf_idx++] = file[i++];

				if (buf_idx == STR_MAX) {
					is_invalid = true;
					goto invalid;
				}

				row->value = cpm_str_n_dup(buf, STR_MAX);

				// Appending to table
				table->n_rows++;
				table->rows =
				    realloc(table->rows,
					    table->n_rows * sizeof(row_t *));
				table->rows[table->n_rows - 1] = row;
			}

			parse.n_tables++;
			parse.tables =
			    realloc(parse.tables,
				    parse.n_tables * sizeof(table_t *));
			parse.tables[parse.n_tables - 1] = table;

			if (i == conf_size) {
				goto end;
			}

		} else {
			is_invalid = true;
			goto invalid;
		}
	}

 invalid:

	if (is_invalid) {
		cpm_error("Invalid Config\n");
		exit(1);
	}

 end:
#ifdef __unix__

	if (file != MAP_FAILED && munmap(file, conf_size) == -1) {
		cpm_error("MUnmap Error\n");
		exit(1);
	}

	if (conf_fd != -1 && close(conf_fd) == -1) {
		cpm_error("Close Error\n");
		exit(1);
	}
#elif _WIN32

	if (file) {
		UnmapViewOfFile(file);
	}

	if (h_map_file) {
		CloseHandle(h_map_file);
	}

	if (h_file != INVALID_HANDLE_VALUE) {
		CloseHandle(h_file);
	}
#endif
}

void
cpm_free_parser()
{

	for (int i = 0; i < parse.n_tables; i++) {

		free(parse.tables[i]->header);
		for (int j = 0; j < parse.tables[i]->n_rows; j++) {
			free(parse.tables[i]->rows[j]->key);
			free(parse.tables[i]->rows[j]->value);
		}
		free(parse.tables[i]);
	}

	memset(&parse, 0, sizeof(parse_t));
}
