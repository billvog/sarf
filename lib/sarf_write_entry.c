#include "sarf_int.h"

int sarf_write_entry(libsarf_archive_t* archive, libsarf_entry_t* entry) {
	if (archive->open_mode == LSARF_READ_ONLY) {
		return LSARF_ERR_CANNOT_WRITE;
	}

	// to prevent absolute paths
	if (entry->filename[0] == '/') {
		entry->filename++;
	}

	fprintf(archive->file, "%04d", LSARF_ARCHIVE_VERSION);
	fprintf(archive->file, "%04lu", strlen(entry->filename));
	fprintf(archive->file, "%s", entry->filename);
	fprintf(archive->file, "%08hu", entry->mode);
	fprintf(archive->file, "%08u", entry->uid);
	fprintf(archive->file, "%08u", entry->gid);
	fprintf(archive->file, "%012lld", entry->size);
	fprintf(archive->file, "%012ld", entry->mod_time);

	return LSARF_OK;
}