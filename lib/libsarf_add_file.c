#include "sarf_int.h"

int libsarf_add_file(libsarf_archive_t* archive, const char* target, const char* destination) {
	if (archive->open_mode == LSARF_READ_ONLY)
		return LSARF_ERR_CANNOT_WRITE;
	
	struct stat target_stat;
	stat(target, &target_stat);

	int target_offset = 0;
	while ((target[target_offset] == '.' && (target[target_offset+1] == '.' || target[target_offset+1] == '/'))
			|| target[target_offset] == '/')
	{
		target_offset++;
	}

	char* target_final = malloc(sizeof(char) * (strlen(target) - target_offset + 1));
	memcpy(target_final, target + target_offset, strlen(target) - target_offset);

	char* final_dest = malloc(sizeof(char) * 100);
	if (destination == NULL || strlen(destination) <= 0) {
		strcpy(final_dest, target_final);
	}
	else {
		if (destination[strlen(destination) - 1] == '/')
			sprintf(final_dest, "%s%s", destination, target_final);
		else strcpy(final_dest, destination);
	}

	if (strlen(final_dest) > PATH_MAX) {
		return LSARF_ERR_FILENAME_MAX;
	}

	if (S_ISREG(target_stat.st_mode)) {
		FILE* target_file = fopen(target, "rb");
		if (target_file == NULL) {
			return LSARF_ERR_CANNOT_OPEN;
		}

		// Write file header to archive
		fprintf(archive->file, "%04lu", strlen(final_dest));
		fprintf(archive->file, "%s", final_dest);
		fprintf(archive->file, "%08hu", target_stat.st_mode);
		fprintf(archive->file, "%08u", target_stat.st_uid);
		fprintf(archive->file, "%08u", target_stat.st_gid);
		fprintf(archive->file, "%012lld", target_stat.st_size);
		fprintf(archive->file, "%012ld", target_stat.st_mtimespec.tv_sec);

		// Write file contents to archive
		char buffer[LSARF_CHUNK_SIZE];
		while (!feof(target_file)) {
			size_t bytes_read = fread(buffer, 1, LSARF_CHUNK_SIZE, target_file);
			fwrite(buffer, 1, bytes_read, archive->file);
		}

		fclose(target_file);
	}
	else if (S_ISDIR(target_stat.st_mode)) {
		strcat(final_dest, "/");
		fprintf(archive->file, "%04lu", strlen(final_dest));
		fprintf(archive->file, "%s", final_dest);
		fprintf(archive->file, "%08hu", target_stat.st_mode);
		fprintf(archive->file, "%08u", target_stat.st_uid);
		fprintf(archive->file, "%08u", target_stat.st_gid);
		fprintf(archive->file, "%012lld", (int64_t) 0);
		fprintf(archive->file, "%012ld", target_stat.st_mtimespec.tv_sec);
	}	

	return LSARF_OK;
}