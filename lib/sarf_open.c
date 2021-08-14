#include "sarf_int.h"

int sarf_open(libsarf_archive_t* archive, const char* filename, sarf_flags_t flags) {
	struct stat archive_stat;
	stat(filename, &archive_stat);

	FILE* archive_file;

	if (flags & LSARF_RDONLY) {
		archive_file = fopen(filename, "rb");	
	}
	else if (flags & LSARF_WRONLY) {
		if (flags & LSARF_TRUNC)
			archive_file = fopen(filename, "wb+");
		else
			archive_file = fopen(filename, "ab+");
	}
	else {
		return LSARF_ERR_INV_FLAGS;
	}

	if (archive_file == NULL) {
		return LSARF_ERR_CANNOT_OPEN;
	}

	archive->filename = strdup(filename);
	archive->file = archive_file;
	archive->open_mode = flags & LSARF_RDONLY ? LSARF_READ_ONLY : LSARF_WRITE_ONLY;
	archive->stat = archive_stat;

	return LSARF_OK;
}