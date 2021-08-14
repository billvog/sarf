#include "sarf_int.h"

int libsarf_count_entries(libsarf_archive_t* archive, int* file_count, const char* search) {
	int count_all = search == NULL ? 0 : -1;
	*file_count = 0;

	fseek(archive->file, 0, SEEK_SET);
	while (ftell(archive->file) < archive->stat.st_size) {
		libsarf_entry_t* file_header = malloc(sizeof(libsarf_entry_t));
		int res = libsarf_read_entry(archive, file_header);
		if (res != 0)
			return res;

		fseek(archive->file, file_header->size, SEEK_CUR);

		if (count_all == 0 || strncmp(file_header->filename, search, strlen(search)) == 0)
			(*file_count)++;

		free(file_header);
	}

	return LSARF_OK;
}