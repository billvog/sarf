#include "sarf.h"

// errors
char* libsarf_err2str(int err) {
	char* error_str = malloc(sizeof(char) * 100);

	if (err == LSARF_A_CANNOT_OPEN)
		strcpy(error_str, "File cannot be opened");
	else
		strcpy(error_str, "Unknown error occured");

	return error_str;
}

// archive
int libsarf_open_archive(libsarf_archive* archive, const char* filename) {
	FILE* archive_file = fopen(filename, "rb");
	if (archive_file == NULL) {
		return LSARF_A_CANNOT_OPEN;
	}

	fclose(archive_file);

	archive->filename = malloc(sizeof(char) * strlen(filename) + 1);
	strcpy(archive->filename, filename);

	return LSARF_OK;
}

int libsarf_add_file_to_archive(libsarf_archive* archive, const char* target) {
	FILE* archive_file = fopen(archive->filename, "ab");
	if (archive_file == NULL) {
		return LSARF_A_CANNOT_OPEN;
	}

	struct stat target_stat;
	stat(target, &target_stat);

	if (S_ISREG(target_stat.st_mode) != 0) {
		return LSARF_NOT_REG_FILE;
	}

	FILE* target_file = fopen(target, "rb");
	if (target_file == NULL) {
		return LSARF_A_CANNOT_OPEN;
	}

	char *target_buffer = malloc(sizeof(char) * (target_stat.st_size) + 1);
	fread(target_buffer, sizeof(char), target_stat.st_size, target_file);

	fclose(target_file);

	// Write data to archive
	fprintf(archive_file, "%-100s", target);
	fprintf(archive_file, "%012lld", target_stat.st_size);
	fprintf(archive_file, "%012ld", target_stat.st_mtimespec.tv_sec);
	fprintf(archive_file, "%-32s", " ");
	fwrite(target_buffer, sizeof(char), target_stat.st_size, archive_file);
	fprintf(archive_file, "%-32s", " ");

	free(target_buffer);

	fclose(archive_file);

	return LSARF_OK;
}









