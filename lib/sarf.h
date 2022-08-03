/*
	sarf.h
	declartions
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
#define LSARF_VERSION          "0.12"
#define LSARF_ARCHIVE_VERSION  1

#ifndef LSARF_BUILT_OS
#define LSARF_BUILT_OS "OS"
#endif

// Configuration
#define LSARF_CHUNK_SIZE 524288

// Errors
#define LSARF_NOK               -1
#define LSARF_OK                 0
#define LSARF_ERR_CANNOT_OPEN    1
#define LSARF_ERR_CANNOT_CREATE  2
#define LSARF_ERR_NOT_FOUND      3
#define LSARF_ERR_FILENAME_MAX   4
#define LSARF_ERR_CANNOT_WRITE   5
#define LSARF_ERR_NOT_EXIST      6
#define LSARF_ERR_INV_FLAGS      7

// Flags
#define LSARF_CREATE   1u
#define LSARF_TRUNC    2u
#define LSARF_RDONLY   3u
#define LSARF_WRONLY   4u

// Types
typedef uint32_t sarf_flags_t;
typedef int libsarf_open_archive_mode_t;
typedef int libsarf_compression_level_t;

// Enums
enum libsarf_open_archive_mode {
	LSARF_READ_ONLY = 1,
	LSARF_WRITE_ONLY
};

// Structs
typedef struct {
	char* filename;
	FILE* file;
	libsarf_open_archive_mode_t open_mode;
	struct stat stat;
	int error;
} libsarf_archive_t;

typedef struct {
	int archived_with_version;
	char* filename;
	mode_t mode;
	uid_t uid;
	gid_t gid;
	int64_t size;
	long mod_time;
} libsarf_entry_t;

// Functions
const char* sarf_err2str(int err);
const char* sarf_errorstr(libsarf_archive_t* archive);

int sarf_open(libsarf_archive_t* archive, const char* filename, sarf_flags_t flags);
int sarf_close(libsarf_archive_t* archive);

int sarf_entry_from_stat(libsarf_entry_t* entry, struct stat stat);
void sarf_free_entry(libsarf_entry_t* entry);
void sarf_entry_set_name(libsarf_entry_t* entry, const char *name);

int sarf_skip_file_data(libsarf_archive_t* archive, libsarf_entry_t* entry);

int sarf_read_entry(libsarf_archive_t* archive, libsarf_entry_t* entry);
int sarf_write_entry(libsarf_archive_t* archive, libsarf_entry_t* entry);

size_t sarf_read(libsarf_archive_t* archive, void *buffer, size_t size);
size_t sarf_write(libsarf_archive_t* archive, void *buffer, size_t size);

#endif
#ifdef __cplusplus
}

#endif