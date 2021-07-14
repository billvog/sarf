#include "sarf.h"

// errors
char* libsarf_err2str(int err) {
	char* error_str = malloc(sizeof(char) * 100);

	switch (err) {
		case LSARF_ERR_A_CANNOT_OPEN:
			strcpy(error_str, "Couldn't open archive file");
			break;
		case LSARF_ERR_T_CANNOT_OPEN:
			strcpy(error_str, "Couldn't open target file");
			break;
		case LSARF_ERR_NOT_REG_FILE:
			strcpy(error_str, "Not regural file");
			break;
		case LSARF_ERR_O_CANNOT_CREATE:
			strcpy(error_str, "Couldn't create output file");
			break;
		case LSARF_ERR_TiA_NOT_FOUND:
			strcpy(error_str, "Target not found in archive");
			break;
		case LSARF_ERR_TMP_CANNOT_CREATE:
			strcpy(error_str, "Couldn't create temp file");
			break;
		case LSARF_OK:
			strcpy(error_str, "");
			break;
		default:
			strcpy(error_str, "Unknown error occured");
			break;
	}

	return error_str;
}

// utils
void libsarf_format_mode(char *str, uint16_t mode) {
	str[0] = (mode & S_IRUSR) ? 'r' : '-';
    str[1] = (mode & S_IWUSR) ? 'w' : '-';
    str[2] = (mode & S_IXUSR) ? 'x' : '-';
    str[3] = (mode & S_IRGRP) ? 'r' : '-';
    str[4] = (mode & S_IWGRP) ? 'w' : '-';
    str[5] = (mode & S_IXGRP) ? 'x' : '-';
    str[6] = (mode & S_IROTH) ? 'r' : '-';
    str[7] = (mode & S_IWOTH) ? 'w' : '-';
    str[8] = (mode & S_IXOTH) ? 'x' : '-';
    str[9] = '\0';
}

void libsarf_format_file_size(char *str, int64_t size) {
	if (size >= 1000000000) {
		sprintf(str, "%lldG", size / 1000000000);
	} else if (size >= 1000000) {
		sprintf(str, "%lldM", size / 1000000);
	} else if (size >= 1000) {
		sprintf(str, "%lldK", size / 1000);
	} else {
		sprintf(str, "%lldB", size);
	}
}

void libsarf_format_epoch(char *str, long timestamp) {
	struct tm ts;
	ts = *localtime(&timestamp);
	strftime(str, sizeof(str), "%d %b %R", &ts);
}

void libsarf_format_uid(char *str, uint16_t uid) {
	struct passwd *pws = getpwuid(uid);
	strcpy(str, pws->pw_name);
}

void libsarf_format_gid(char *str, uint16_t gid) {
	struct group *grp = getgrgid(gid);
	strcpy(str, grp->gr_name);
}

// archive
int libsarf_open_archive(libsarf_archive* archive, const char* filename) {
	FILE* archive_file = fopen(filename, "ab+");
	if (archive_file == NULL) {
		return LSARF_ERR_A_CANNOT_OPEN;
	}

	struct stat* archive_stat = malloc(sizeof(struct stat));
	stat(filename, archive_stat);

	archive->filename = malloc(sizeof(char) * strlen(filename) + 1);
	strcpy(archive->filename, filename);

	archive->file = archive_file;
	archive->stat = archive_stat;

	return LSARF_OK;
}

int libsarf_close_archive(libsarf_archive* archive) {
	fclose(archive->file);
	free(archive->stat);
	return LSARF_OK;
}

int libsarf_add_file_to_archive(libsarf_archive* archive, const char* target) {
	struct stat target_stat;
	stat(target, &target_stat);

	if (!S_ISREG(target_stat.st_mode)) {
		return LSARF_ERR_NOT_REG_FILE;
	}

	FILE* target_file = fopen(target, "rb");
	if (target_file == NULL) {
		return LSARF_ERR_T_CANNOT_OPEN;
	}

	char *target_buffer = malloc(sizeof(char) * (target_stat.st_size) + 1);
	fread(target_buffer, sizeof(char), target_stat.st_size, target_file);

	fclose(target_file);

	// Write data to archive
	fprintf(archive->file, "%-100s", target);
	fprintf(archive->file, "%08hu", target_stat.st_mode);
	fprintf(archive->file, "%08u", target_stat.st_uid);
	fprintf(archive->file, "%08u", target_stat.st_gid);
	fprintf(archive->file, "%012lld", target_stat.st_size);
	fprintf(archive->file, "%012ld", target_stat.st_mtimespec.tv_sec);
	fwrite(target_buffer, sizeof(char), target_stat.st_size, archive->file);

	free(target_buffer);
	return LSARF_OK;
}

int libsarf_extract_file_from_archive(libsarf_archive* archive, const char* target, const char* output) {
  	int file_found = -1;
  	fseek(archive->file, 0, SEEK_SET);
	while (ftell(archive->file) < archive->stat->st_size) {
		char *file_name = malloc(sizeof(char) * LSARF_FILENAME_MAX);
		fread(file_name, 1, LSARF_FILENAME_MAX, archive->file);

		// clear filename from whitespaces
		char* fn_back = file_name + strlen(file_name);
	    while (isspace(*--fn_back));
	    *(fn_back+1) = '\0';

	    // file mode
	    char *file_mode_str = malloc(sizeof(char) * 8);
		fread(file_mode_str, 1, 8, archive->file);
		uint16_t file_mode = atoi(file_mode_str);

	    // skip stuff we don't want
		fseek(archive->file, 8 + 8, SEEK_CUR);

		char *file_size_str = malloc(sizeof(char) * 12);
		fread(file_size_str, 1, 12, archive->file);
		int file_size = atoi(file_size_str);

		if (strcmp(file_name, target) != 0) {
			fseek(archive->file, 12 + file_size, SEEK_CUR);
			continue;
		}

		file_found = 0;

		fseek(archive->file, 12, SEEK_CUR);

		char *file_buffer = malloc(sizeof(char) * (file_size + 1));
		fread(file_buffer, sizeof(char), file_size, archive->file);

		FILE* output_file;
		int output_fd = open(output, O_WRONLY | O_CREAT, file_mode);
		output_file = fdopen(output_fd, "w");

		if (output_file == NULL) {
			return LSARF_ERR_O_CANNOT_CREATE;
		}

		fwrite(file_buffer, sizeof(char), file_size, output_file);

		free(file_buffer);
		fclose(output_file);

		break;
	}

	if (file_found == -1)
		return LSARF_ERR_TiA_NOT_FOUND;

	return LSARF_OK;
}

int libsarf_count_files_in_archive(libsarf_archive* archive, int* file_count) {
	*file_count = 0;

  	fseek(archive->file, 0, SEEK_SET);
	while (ftell(archive->file) < archive->stat->st_size) {
		(*file_count)++;

		fseek(archive->file, LSARF_FILENAME_MAX + 8 + 8 + 8, SEEK_CUR);

		// file size
		char *file_size_str = malloc(sizeof(char) * 12);
		fread(file_size_str, 1, 12, archive->file);
		int64_t file_size = atoi(file_size_str);

		fseek(archive->file, 12 + file_size, SEEK_CUR);
	}

	return LSARF_OK;
}

int libsarf_stat_files_from_archive(libsarf_archive* archive, libsarf_stat_file*** stat_files) {
  	int file_count = 0;
  	fseek(archive->file, 0, SEEK_SET);
	while (ftell(archive->file) < archive->stat->st_size) {
		// filename
		char *file_name = malloc(sizeof(char) * LSARF_FILENAME_MAX);
		fread(file_name, 1, LSARF_FILENAME_MAX, archive->file);

		// clear filename from whitespaces
		char* fn_back = file_name + strlen(file_name);
	    while (isspace(*--fn_back));
	    *(fn_back+1) = '\0';

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

		fseek(archive->file, file_size, SEEK_CUR);

		libsarf_stat_file* stat = malloc(sizeof(libsarf_stat_file));

		stat->filename = malloc(sizeof(char) * strlen(file_name) + 1);
		strcpy(stat->filename, file_name);

		stat->mode = file_mode;
		stat->uid = file_uid;
		stat->gid = file_gid;
		stat->size = file_size;
		stat->mod_time = file_mtime;

		(*stat_files)[file_count] = stat;
		file_count++;
	}

	return LSARF_OK;
}

int libsarf_remove_file_from_archive(libsarf_archive* archive, const char* target) {
	char *temp_filename = malloc(sizeof(char) * 100);
	sprintf(temp_filename, "%d_temp.sarf", rand() * 1000);

	FILE* temp_ar = fopen(temp_filename, "wb");
	if (temp_ar == NULL) {
		return LSARF_ERR_TMP_CANNOT_CREATE;
	}

	int file_found = -1;
  	fseek(archive->file, 0, SEEK_SET);
	while (ftell(archive->file) < archive->stat->st_size) {
		// filename
		char *file_name = malloc(sizeof(char) * LSARF_FILENAME_MAX);
		fread(file_name, 1, LSARF_FILENAME_MAX, archive->file);

		// clear filename from whitespaces
		char* fn_back = file_name + strlen(file_name);
	    while (isspace(*--fn_back));
	    *(fn_back+1) = '\0';

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

		// if this is the target exclude it from the new archive
		if (strcmp(file_name, target) == 0) {
			file_found = 0;
			fseek(archive->file, file_size, SEEK_CUR);
			continue;
		}

		char *file_buffer = malloc(sizeof(char) * (file_size + 1));
		fread(file_buffer, sizeof(char), file_size, archive->file);

		fprintf(temp_ar, "%-100s", file_name);
		fprintf(temp_ar, "%08hu", file_mode);
		fprintf(temp_ar, "%08u", file_uid);
		fprintf(temp_ar, "%08u", file_gid);
		fprintf(temp_ar, "%012lld", file_size);
		fprintf(temp_ar, "%012ld", file_mtime);
		fwrite(file_buffer, sizeof(char), file_size, temp_ar);

		free(file_buffer);
	}

	if (file_found == -1) {
		remove(temp_filename);
		return LSARF_ERR_TiA_NOT_FOUND;
	}

	if (rename(temp_filename, archive->filename) != 0) {
		return LSARF_ERR_O_CANNOT_CREATE;
	}

	return LSARF_OK;
}