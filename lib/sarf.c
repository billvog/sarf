#include "sarf.h"

// errors
char* libsarf_err2str(int err) {
	char* error_str = malloc(sizeof(char) * 100);

	if (err == LSARF_A_CANNOT_OPEN)
		strcpy(error_str, "Archive file cannot be opened");
	else if (err == LSARF_T_CANNOT_OPEN)
		strcpy(error_str, "Target file cannot be opened");
	else if (err == LSARF_NOT_REG_FILE)
		strcpy(error_str, "Not regural file");
	else if (err == LSARF_O_CANNOT_CREATE)
		strcpy(error_str, "Cannot create output file");
	else if (err == LSARF_TiA_NOT_FOUND)
		strcpy(error_str, "Target not found in archive");
	else if (err == LSARF_OK) strcpy(error_str, "");
	else strcpy(error_str, "Unknown error occured");

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

	if (!S_ISREG(target_stat.st_mode)) {
		return LSARF_NOT_REG_FILE;
	}

	FILE* target_file = fopen(target, "rb");
	if (target_file == NULL) {
		return LSARF_T_CANNOT_OPEN;
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

int libsarf_extract_file_from_archive(libsarf_archive* archive, const char* target, const char* dest) {
	FILE* archive_file = fopen(archive->filename, "r");
	if (archive_file == NULL) {
		return LSARF_A_CANNOT_OPEN;
	}

	struct stat archive_stat;
  	stat(archive->filename, &archive_stat);

  	int file_found = -1;
	while (ftell(archive_file) < archive_stat.st_size) {
		char *file_name = malloc(sizeof(char) * LSARF_FILENAME_MAX);
		fread(file_name, 1, LSARF_FILENAME_MAX, archive_file);

		// clear filename from whitespaces
		char* fn_back = file_name + strlen(file_name);
	    while (isspace(*--fn_back));
	    *(fn_back+1) = '\0';

		char *file_size_str = malloc(sizeof(char) * 12);
		fread(file_size_str, 1, 12, archive_file);
		int file_size = atoi(file_size_str);

		if (strcmp(file_name, target) != 0) {
			fseek(archive_file, 12 + 32 + file_size + 32, SEEK_CUR);
			continue;
		}

		file_found = 0;

		fseek(archive_file, 12, SEEK_CUR);
		fseek(archive_file, 32, SEEK_CUR);

		char *file_buffer = malloc(sizeof(char) * (file_size + 1));
		fread(file_buffer, sizeof(char), file_size, archive_file);

		fseek(archive_file, 32, SEEK_CUR);

		FILE* output_file = fopen(dest, "w");
		if (output_file == NULL) {
			return LSARF_O_CANNOT_CREATE;
		}

		fwrite(file_buffer, sizeof(char), file_size, output_file);

		free(file_buffer);
		fclose(output_file);

		break;
	}

	if (file_found == -1)
		return LSARF_TiA_NOT_FOUND;

	return LSARF_OK;
}

int libsarf_stat_files_from_archive(libsarf_archive* archive, libsarf_stat_file* stat_files[], int* file_count) {
	FILE* archive_file = fopen(archive->filename, "r");
	if (archive_file == NULL) {
		return LSARF_A_CANNOT_OPEN;
	}

	struct stat archive_stat;
  	stat(archive->filename, &archive_stat);

  	*file_count = 0;

	while (ftell(archive_file) < archive_stat.st_size) {
		(*file_count)++;

		char *file_name = malloc(sizeof(char) * LSARF_FILENAME_MAX);
		fread(file_name, 1, LSARF_FILENAME_MAX, archive_file);

		// clear filename from whitespaces
		char* fn_back = file_name + strlen(file_name);
	    while (isspace(*--fn_back));
	    *(fn_back+1) = '\0';

		char *file_size_str = malloc(sizeof(char) * 12);
		fread(file_size_str, 1, 12, archive_file);
		int16_t file_size = atoi(file_size_str);

		char *file_mtime_str = malloc(sizeof(char) * 12);
	    fread(file_mtime_str, 1, 12, archive_file);
	    int16_t file_mtime = atol(file_mtime_str);

		fseek(archive_file, 32, SEEK_CUR);
		fseek(archive_file, file_size, SEEK_CUR);
		fseek(archive_file, 32, SEEK_CUR);

		libsarf_stat_file* stat = stat_files[*file_count];
		stat = malloc(sizeof(libsarf_stat_file));

		stat->filename = malloc(sizeof(char) * strlen(file_name) + 1);
		strcpy(stat->filename, file_name);

		stat->size = file_size;
		stat->mod_time = file_mtime;
	}

	return LSARF_OK;
}





