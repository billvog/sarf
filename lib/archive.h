#ifndef __SARF_ARCHIVE__
#define __SARF_ARCHIVE__

#include "sarf.h"

typedef struct {
	char* filename;
	int error;
} libsarf_archive;

typedef struct {
	char* filename;
	int16_t size;
	int16_t mod_time;
} libsarf_stat_file;

int libsarf_open_archive(libsarf_archive* archive, const char* filename);

int libsarf_add_file_to_archive(libsarf_archive* archive, const char* target);
int libsarf_extract_file_from_archive(libsarf_archive* archive, const char* target, const char* dest);

libsarf_stat_file** libsarf_stat_files_from_archive(libsarf_archive* archive, int* file_count);

#endif