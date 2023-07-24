#include <string.h>
#include <stdlib.h>

#include <global.h>

char *CPM_GLOBAL_STORE;

void
cpm_init_globals()
{
	// Global Store
	CPM_GLOBAL_STORE = NULL;
	CPM_GLOBAL_STORE = strdup(getenv("HOME"));
	CPM_GLOBAL_STORE =
	    realloc(CPM_GLOBAL_STORE,
		    strlen(CPM_GLOBAL_STORE) +
		    strlen("/" CPM_REL_GLOBAL_STORE) + 1);
	strcat(CPM_GLOBAL_STORE, "/" CPM_REL_GLOBAL_STORE);
}

void
cpm_free_globals()
{
	free(CPM_GLOBAL_STORE);
	CPM_GLOBAL_STORE = NULL;
}
