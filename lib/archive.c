#include "archive.h"

int libsarf_open_archive(libsarf_archive* archive, const char* filename) {
	return LSARF_A_CANNOT_OPEN;
}

int libsarf_add_file_to_archive(libsarf_archive* archive, const char* target_file) {
	printf("Adding %s to archive %s\n", target_file, archive->filename);
	return 0;
}