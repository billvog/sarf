#include "sarf_int.h"

int sarf_read_entry(libsarf_archive_t* archive, libsarf_entry_t* entry) {
	if (ftell(archive->file) >= archive->stat.st_size || feof(archive->file))
		return LSARF_NOK;

	// archived with version
	char *archive_version_str = malloc(sizeof(char) * 4);
	fread(archive_version_str, 1, 4, archive->file);
	int archive_version = atoi(archive_version_str);

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

	entry->archived_with_version = archive_version;
	entry->filename = strdup(file_name);
	entry->mode = file_mode;
	entry->uid = file_uid;
	entry->gid = file_gid;
	entry->size = file_size;
	entry->mod_time = file_mtime;

	return LSARF_OK;
}