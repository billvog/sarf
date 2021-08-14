#include "sarf_int.h"

int libsarf_open(libsarf_archive_t* archive, const char* filename, sarf_flags_t flags) {
	struct stat archive_stat;
	int exists = stat(filename, &archive_stat);
	
	if (!(flags & LSARF_CREATE) && exists != 0) {
		return LSARF_ERR_NOT_EXISTS;
	}

	FILE* archive_file;

	if ((flags & LSARF_RDONLY) == 0) {
		archive_file = fopen(filename, "rb");	
	}
	else {
		if (flags & LSARF_TRUNC)
			archive_file = fopen(filename, "wb+");
		else
			archive_file = fopen(filename, "ab+");
	}

	if (archive_file == NULL) {
		return LSARF_ERR_CANNOT_OPEN;
	}

	archive->filename = strdup(filename);
	archive->file = archive_file;
	archive->open_mode = flags & LSARF_READ_ONLY ? LSARF_READ_ONLY : LSARF_WRITE;
	archive->stat = archive_stat;

	return LSARF_OK;
}