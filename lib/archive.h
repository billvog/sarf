#ifndef __SARF_ARCHIVE__
#define __SARF_ARCHIVE__

#include "sarf.h"

typedef struct {
	char* filename;
} libsarf_archive;

int libsarf_open_archive(libsarf_archive* archive, const char* filename);

int libsarf_add_file_to_archive(libsarf_archive* archive, const char* target);
int libsarf_extract_file_from_archive(libsarf_archive* archive, const char* target, const char* dest);

#endif