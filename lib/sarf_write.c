#include "sarf_int.h"

size_t sarf_write(libsarf_archive_t* archive, void *buffer, size_t size) {
	if (archive->open_mode == LSARF_READ_ONLY) {
		archive->error = LSARF_ERR_CANNOT_WRITE;
		return -1;
	}
	
	return fwrite(buffer, size, 1, archive->file);
}