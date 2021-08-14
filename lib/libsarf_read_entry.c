#include "sarf_int.h"

int libsarf_read_entry(libsarf_archive_t* archive, libsarf_entry_t* file_header) {
	if (!feof(archive->file)) {
		// filename size
		char *filename_size_str = malloc(sizeof(char) * 4);
		fread(filename_size_str, 1, 4, archive->file);
		uint16_t filename_size = atoi(filename_size_str);

		// filename
		char *file_name = malloc(sizeof(char) * filename_size);
		fread(file_name, 1, filename_size, archive->file);

		// file mode
		char *file_mode_str = malloc(sizeof(char) * 8);
		fread(file_mode_str, 1, 8, archive->file);
		uint16_t file_mode = atoi(file_mode_str);

		// owner id
		char *file_uid_str = malloc(sizeof(char) * 8);
		fread(file_uid_str, 1, 8, archive->file);
		uint16_t file_uid = atoi(file_uid_str);

		// group id
		char *file_gid_str = malloc(sizeof(char) * 8);
		fread(file_gid_str, 1, 8, archive->file);
		uint16_t file_gid = atoi(file_gid_str);

		// file size
		char *file_size_str = malloc(sizeof(char) * 12);
		fread(file_size_str, 1, 12, archive->file);
		int64_t file_size = atoi(file_size_str);

		// last mod time
		char *file_mtime_str = malloc(sizeof(char) * 12);
		fread(file_mtime_str, 1, 12, archive->file);
		long file_mtime = atol(file_mtime_str);

		file_header->filename = file_name;
		file_header->mode = file_mode;
		file_header->uid = file_uid;
		file_header->gid = file_gid;
		file_header->size = file_size;
		file_header->mod_time = file_mtime;
	}
	else {
		file_header = NULL;
		return LSARF_NOK;
	}

	return LSARF_OK;
}