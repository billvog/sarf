#ifndef __SARF_ARCHIVE__
#define __SARF_ARCHIVE__

#include "libsarf.h"
#include "flags.h"

// limits
#define LSARF_FILENAME_MAX 100

// structs
typedef struct {
	char* filename;
	FILE* file;
	struct stat* stat;
	int error;
} libsarf_archive_t;

typedef struct {
	char* filename;
	uint16_t mode;
	uint16_t uid;
	uint16_t gid;
	int64_t size;
	long mod_time;
} libsarf_file_t;

// functions
int libsarf_open_archive(libsarf_archive_t* archive, const char* filename);
int libsarf_close_archive(libsarf_archive_t* archive);

int libsarf_add_file_to_archive(libsarf_archive_t* archive, const char* target, const char* destination);
int libsarf_add_dir_to_archive(libsarf_archive_t* archive, const char* target_dir, const char* destination, sarf_flags_t flags);
int libsarf_remove_file_from_archive(libsarf_archive_t* archive, const char* target);

int libsarf_extract_all_from_archive(libsarf_archive_t* archive, const char* output);
int libsarf_extract_file_from_archive(libsarf_archive_t* archive, const char* target, const char* output);

int libsarf_count_files_in_archive(libsarf_archive_t* archive, int* file_count);
int libsarf_stat_files_from_archive(libsarf_archive_t* archive, libsarf_file_t*** stat_files);

#endif