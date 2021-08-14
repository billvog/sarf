#include "sarf_int.h"

size_t sarf_read(libsarf_archive_t* archive, void *buffer, size_t size) {
	return fread(buffer, size, 1, archive->file);
}