#include "sarf_int.h"

int libsarf_write_entry(libsarf_archive_t* archive, libsarf_entry_t* file_header) {
	if (file_header->filename[0] == '/') {
		file_header->filename++;
	}

	fprintf(archive->file, "%04lu", strlen(file_header->filename));
	fprintf(archive->file, "%s", file_header->filename);
	fprintf(archive->file, "%08hu", file_header->mode);
	fprintf(archive->file, "%08u", file_header->uid);
	fprintf(archive->file, "%08u", file_header->gid);
	fprintf(archive->file, "%012lld", file_header->size);
	fprintf(archive->file, "%012ld", file_header->mod_time);

	return LSARF_OK;
}