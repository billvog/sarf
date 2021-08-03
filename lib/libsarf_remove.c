#include "sarf_int.h"

int libsarf_remove(libsarf_archive_t* archive, const char* target) {
	if (archive->open_mode == LSARF_READ_ONLY)
		return LSARF_ERR_CANNOT_WRITE;

	char *temp_filename = malloc(sizeof(char) * 100);
	sprintf(temp_filename, "%s.sarf.%d", basename(archive->filename), abs(rand() * 1000));

	FILE* temp_ar = fopen(temp_filename, "wb");
	if (temp_ar == NULL) {
		return LSARF_ERR_CANNOT_CREATE;
	}

	int file_found = -1;
	fseek(archive->file, 0, SEEK_SET);
	while (ftell(archive->file) < archive->stat.st_size) {
		libsarf_entry_t* file_header = malloc(sizeof(libsarf_entry_t));
		int res = _libsarf_read_entry(archive, file_header);
		if (res != 0)
			return res;

		// if this is the target exclude it from the new archive
		if (strcmp(file_header->filename, target) == 0 ||
			(target[strlen(target)-1] == '/' && strncmp(file_header->filename, target, strlen(target)) == 0))
		{
			file_found = 0;
			free(file_header);
			fseek(archive->file, file_header->size, SEEK_CUR);
			continue;
		}

		fprintf(temp_ar, "%04lu", strlen(file_header->filename));
		fprintf(temp_ar, "%s", file_header->filename);
		fprintf(temp_ar, "%08hu", file_header->mode);
		fprintf(temp_ar, "%08u", file_header->uid);
		fprintf(temp_ar, "%08u", file_header->gid);
		fprintf(temp_ar, "%012lld", file_header->size);
		fprintf(temp_ar, "%012ld", file_header->mod_time);

		int64_t bytes_left = file_header->size;
		char buffer[LSARF_CHUNK_SIZE];
		while (!feof(archive->file)) {
			size_t read_size = LSARF_CHUNK_SIZE;
			if (bytes_left < LSARF_CHUNK_SIZE) {
				read_size = bytes_left;
			}

			size_t bytes_read = fread(buffer, 1, read_size, archive->file);
			bytes_left -= bytes_read;

			fwrite(buffer, 1, bytes_read, temp_ar);

			if (bytes_left <= 0)
				break;
		}

		free(file_header);
	}

	fclose(temp_ar);

	if (file_found == -1) {
		remove(temp_filename);
		return LSARF_ERR_NOT_FOUND;
	}

	if (rename(temp_filename, archive->filename) != 0) {
		return LSARF_ERR_CANNOT_CREATE;
	}

	return LSARF_OK;
}