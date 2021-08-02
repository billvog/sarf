#include "sarf_int.h"

int libsarf_close(libsarf_archive_t* archive) {
	free(archive->filename);

	fclose(archive->file);
	free(archive);

	return LSARF_OK;
}