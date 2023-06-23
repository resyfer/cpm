#ifndef __CPM_PARSER_H
#define __CPM_PARSER_H

/*

Have to make own parser to be cross-platform.

Language of the config is like this:

[a-zA-Z][a-zA-Z0-9]* 	TEXT
=						EQUAL
\n 						LF
[ ]+					WS
\[TEXT\]				HEADER

GRAMMAR -> TABLE GRAMMAR | TABLE LF GRAMMAR | GRAMMAR
TABLE -> HEADER LF COLLECTION | HEADER
COLLECTION -> ROW LF COLLECTION | ROW
ROW -> TEXT WS EQUAL WS TEXT |
	   TEXT EQUAL WS TEXT |
	   TEXT WS EQUAL TEXT |
	   TEXT EQUAL TEXT

*/

#ifdef __unix__
#include <stddef.h>
#elif _WIN32
#include <windows.h>
#endif

typedef struct row {
	char *key;
	char *value;
} row_t;

typedef struct table {
	char *header;
	row_t **rows;
	size_t n_rows;
} table_t;

typedef struct config {
	table_t **tables;
	size_t n_tables;
} parse_t;

void parser();

void free_parser();

#endif
