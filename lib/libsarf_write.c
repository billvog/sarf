#include "sarf_int.h"

size_t libsarf_write(libsarf_archive_t* archive, void *buffer, size_t size) {
	return fwrite(buffer, size, 1, archive->file);
}