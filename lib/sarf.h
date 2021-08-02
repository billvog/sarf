/*
	sarf.h – declartions
*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SARF_LIB
#define SARF_LIB

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <libgen.h>
#include <dirent.h>
#include <inttypes.h>
#include <time.h>

// Library built info
#define LSARF_VERSION "0.0.12"

#ifndef LSARF_BUILT_OS
#define LSARF_BUILT_OS "OS"
#endif

// Configuration
#define LSARF_CHUNK_SIZE 524288 // 512KB

// Errors
#define LSARF_OK 						0
#define LSARF_ERR_A_CANNOT_OPEN			1
#define LSARF_ERR_T_CANNOT_OPEN			2
#define LSARF_ERR_NOT_REG_FILE	 		3
#define LSARF_ERR_O_CANNOT_CREATE		4
#define LSARF_ERR_TiA_NOT_FOUND			5
#define LSARF_ERR_TMP_CANNOT_CREATE		6
#define LSARF_ERR_D_INVALID				7
#define LSARF_ERR_T_FILENAME_MAX		8
#define LSARF_ERR_A_CANNOT_WRITE		9
#define LSARF_ERR_A_NOT_EXISTS			10

// Flags
#define LSARF_CREATE					1u
#define LSARF_TRUNC						2u
#define LSARF_RDONLY					3u
#define LSARF_RECURSIVE 				4u

// Types
typedef uint32_t sarf_flags_t;

typedef int libsarf_open_archive_mode_t;
enum libsarf_open_archive_mode {
	LSARF_READ_ONLY = 1,
	LSARF_WRITE
};

typedef struct {
	char* filename;
	FILE* file;
	libsarf_open_archive_mode_t open_mode;
	struct stat stat;
	int error;
} libsarf_archive_t;

typedef struct {
	char* filename;
	uint16_t mode;
	uint16_t uid;
	uint16_t gid;
	int64_t size;
	long mod_time;
} libsarf_entry_t;

// Functions
int libsarf_init();
const char* libsarf_err2str(int err);

int libsarf_open(libsarf_archive_t* archive, const char* filename, sarf_flags_t flags);
int libsarf_close(libsarf_archive_t* archive);

int libsarf_add_file(libsarf_archive_t* archive, const char* target, const char* destination);
int libsarf_add_dir(libsarf_archive_t* archive, const char* target_dir, const char* destination, sarf_flags_t flags);
int libsarf_remove(libsarf_archive_t* archive, const char* target);

int libsarf_extract_all(libsarf_archive_t* archive, const char* output);
int libsarf_extract(libsarf_archive_t* archive, const char* target, const char* output);

int libsarf_count_entries(libsarf_archive_t* archive, int* file_count, const char* search);
int libsarf_stat(libsarf_archive_t* archive, libsarf_entry_t* stat, int index);

#endif
#ifdef __cplusplus
}

#endif