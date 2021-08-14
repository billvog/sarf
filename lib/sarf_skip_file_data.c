#include "sarf_int.h"

int sarf_skip_file_data(libsarf_archive_t* archive, libsarf_entry_t* entry) {
	return fseek(archive->file, entry->size, SEEK_CUR);
}