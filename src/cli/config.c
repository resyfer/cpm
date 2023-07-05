#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <global.h>
#include <config.h>
#include <parser.h>

config_t config;

void
cpm_load_config()
{
	memset(&config, 0, sizeof(config_t));

	cpm_parser();

	for (int i = 0; i < parse.n_tables; i++) {

		table_t *table = parse.tables[i];

		if (!strcmp(table->header, "global")) {

			for (int j = 0; j < table->n_rows; j++) {

				row_t *row = table->rows[j];
				char *token = NULL, *str = NULL;

				if (!strcmp(row->key, "policy")) {
					config.policy_version =
					    atoi(row->value);

					if (config.policy_version >
					    MAX_POLICY_VERSION) {
						cpm_error
						    ("Incorrect policy version: %s",
						     row->value);
						exit(1);
					}
				} else if (!strcmp(row->key, "type")) {

					if (!strcmp(row->value, "lib-static")) {
						config.type = LIB_STATIC;
					} else
					    if (!strcmp
						(row->value, "lib-dynamic")) {
						config.type = LIB_DYNAMIC;
					} else if (!strcmp(row->value, "bin")) {
						config.type = BINARY;
					} else {
						cpm_error
						    ("Incorrect type value: %s",
						     row->value);
						exit(1);
					}

				} else if (!strcmp(row->key, "compiler")) {

					strncpy(config.compiler, row->value,
						PATH_LEN_MAX - 1);

				} else if (!strcmp(row->key, "headers")) {

					for (token =
					     strtok_r(row->value, ",",
						      &str); token;
					     token =
					     strtok_r(NULL, ",", &str)) {
						if (config.n_headeer_paths ==
						    HDR_NUM_MAX) {
							cpm_error
							    ("Too manu header paths\n");
							exit(1);
						}

						strncpy(config.header_paths
							[config.
							 n_headeer_paths],
							token,
							PATH_LEN_MAX - 1);
						config.n_headeer_paths++;
					}

				} else if (!strcmp(row->key, "src")) {

					for (token =
					     strtok_r(row->value, ",",
						      &str); token;
					     token =
					     strtok_r(NULL, ",", &str)) {

						if (config.n_src_paths ==
						    SRC_NUM_MAX) {
							cpm_error
							    ("Too manu src paths\n");
							exit(1);
						}

						strncpy(config.src_paths
							[config.n_src_paths],
							token,
							PATH_LEN_MAX - 1);
						config.n_src_paths++;
					}

				} else if (!strcmp(row->key, "out")) {
					strncpy(config.output_file, row->value,
						PATH_LEN_MAX - 5);
				} else {
					cpm_error("Incorrect key value: %s",
						  row->key);
				}

			}

		} else if (!strcmp(table->header, "dependencies")) {

			for (int j = 0; j < table->n_rows; j++) {
				char *token = NULL, *str = NULL;
				row_t *row = table->rows[j];

				// Check if duplicate
				for (int k = 0; k < config.n_deps; k++) {

					if (!strncmp
					    (row->key,
					     config.deps[k].package_name,
					     PKG_NAME_LEN_MAX)) {
						cpm_warning
						    ("Ignoring duplicate dependency: %s",
						     row->key);
						goto next;
					}
				}

				strncpy(config.deps[config.n_deps].package_name,
					row->key, PKG_NAME_LEN_MAX - 1);

				char *val = row->value;
				if (val[0] == '^') {
					config.deps[config.n_deps].
					    update_policy = PACTHES_ONLY;
					val++;
				} else if (val[0] == '~') {
					config.deps[config.n_deps].
					    update_policy = MINOR_FIXES_ONLY;
					val++;
				} else if (val[0] == '*') {
					config.deps[config.n_deps].
					    update_policy = LATEST;
				} else if ('9' >= val[0] && val[0] >= '0') {
					config.deps[config.n_deps].
					    update_policy = FIXED;
				}

				if (config.deps[config.n_deps].update_policy !=
				    FIXED) {
					int state = 0;

					for (token =
					     strtok_r(row->value, ",",
						      &str); token;
					     token =
					     strtok_r(NULL, ".", &str)) {
						int ver = atoi(token);

						if (state == 0) {
							config.deps
							    [config.n_deps].
							    version.major = ver;
						} else if (state == 1) {
							config.deps
							    [config.n_deps].
							    version.minor = ver;
						} else if (state == 2) {
							config.deps
							    [config.n_deps].
							    version.patch = ver;
						}

						state++;

					}
				}

				config.n_deps++;
 next:
				;	//empty statement to keep linter happy
			}

		} else if (!strcmp(table->header, "versions")) {

			for (int j = 0; j < table->n_rows; j++) {

				row_t *row = table->rows[j];

				char *token = NULL, *str = NULL;
				int state = 0;

				for (token =
				     strtok_r(row->key, ",",
					      &str); token;
				     token = strtok_r(NULL, ".", &str)) {
					int ver = atoi(token);

					if (state == 0) {
						config.deps
						    [config.n_deps].
						    version.major = ver;
					} else if (state == 1) {
						config.deps
						    [config.n_deps].
						    version.minor = ver;
					} else if (state == 2) {
						config.deps
						    [config.n_deps].
						    version.patch = ver;
					}

					state++;

				}

				strncpy(config.vers[config.n_vers].commit,
					row->value, 40);

				config.n_vers++;
			}

		} else {
			cpm_warning("Incorrect header: %s", table->header);
			exit(1);
		}

	}

	cpm_free_parser();

}

void
cpm_write_config()
{

}

// 469
