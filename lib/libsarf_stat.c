#include "sarf_int.h"

int libsarf_stat(libsarf_archive_t* archive, libsarf_entry_t* stat, int index) {
	int curr_index = 0;
	int entry_found = -1;

	fseek(archive->file, 0, SEEK_SET);
	while (ftell(archive->file) < archive->stat.st_size) {
		curr_index++;

		libsarf_entry_t* file_header = malloc(sizeof(libsarf_entry_t));
		int res = _libsarf_read_entry(archive, file_header);
		if (res != 0)
			return res;
			
		fseek(archive->file, file_header->size, SEEK_CUR);

		if (curr_index == index) {
			libsarf_entry_t* entry = malloc(sizeof(libsarf_entry_t));
			entry->filename = strdup(file_header->filename);
			entry->mode = file_header->mode;
			entry->uid = file_header->uid;
			entry->gid = file_header->gid;
			entry->size = file_header->size;
			entry->mod_time = file_header->mod_time;

			stat = entry;
			entry_found = 0;
		}

		free(file_header);
	}

	// if (entry_found == -1);

	return LSARF_OK;
}