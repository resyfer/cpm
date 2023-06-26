#ifndef __CPM_LOBAL_H
#define __CPM_LOBAL_H

#include <stdbool.h>
#include <sys/types.h>

#include <args.h>
#include <log.h>
#include <parser.h>

#define POLICY "1"

#define STR_MAX 2048
#define LONG_STR_MAX 2 * STR_MAX
#define PATH_LEN_MAX 260	// Windows limit
#define SRC_NUM_MAX 256
#define HDR_NUM_MAX 256
#define DEPS_NUM_MAX 512
#define VERS_NUM_MAX 512
#define PKG_NAME_LEN_MAX 128
#define BRANCH_NAME_LEN_MAX 256
#define BUILD_FILES_MAX 512

#define CONFIG_FILE_NAME "cpm.build"
#define CPM_DIRECTORY "cpm_modules"

#define MAX_POLICY_VERSION 1

// Config
typedef struct {
	u_int8_t major;
	u_int8_t minor;
	u_int8_t patch;
} sem_ver_t;

typedef enum {
	FIXED,			// Keeps the package version fixed
	PACTHES_ONLY,		// Only updates the package as long as patches are applied. Default
	MINOR_FIXES_ONLY,	// Only updates the package for minor fixes or patches
	LATEST,			// Always latest version
} VERSION_UPDATE_POLICY;

typedef enum {
	REGULAR_FILE,
	DIRECTORY,
	UNKNOWN
} FILE_TYPE;

typedef enum {
	LIB_STATIC,
	LIB_DYNAMIC,
	BINARY
} OUTPUT_TYPE;

typedef struct {
	char package_name[PKG_NAME_LEN_MAX];
	sem_ver_t version;
	VERSION_UPDATE_POLICY update_policy;
} dependency_t;

typedef struct {
	sem_ver_t version;
	char commit[41];	//40-character long full-hash of commit
} version_t;

typedef struct {

	// Global
	u_int8_t policy_version;	// Policy version of config
	OUTPUT_TYPE type;
	char compiler[PATH_LEN_MAX];

	char header_paths[HDR_NUM_MAX][PATH_LEN_MAX];
	int n_headeer_paths;

	char src_paths[HDR_NUM_MAX][PATH_LEN_MAX];
	int n_src_paths;

	char output_file[PATH_LEN_MAX - 4];	// '.exe' has size 4

	// Dependencies
	dependency_t deps[DEPS_NUM_MAX];
	int n_deps;

	// Versions
	version_t vers[VERS_NUM_MAX];
	int n_vers;

} config_t;

extern args_t args;
extern config_t config;
extern parse_t parse;

#endif
