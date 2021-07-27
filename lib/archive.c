#include "archive.h"

int libsarf_open_archive(libsarf_archive_t* archive, const char* filename) {
	FILE* archive_file = fopen(filename, "ab+");
	if (archive_file == NULL) {
		return LSARF_ERR_A_CANNOT_OPEN;
	}

	struct stat archive_stat;
	stat(filename, &archive_stat);

	archive->filename = malloc(sizeof(char) * strlen(filename) + 1);
	strcpy(archive->filename, filename);

	archive->file = archive_file;
	archive->stat = archive_stat;

	return LSARF_OK;
}

int libsarf_close_archive(libsarf_archive_t* archive) {
	free(archive->filename);

	fclose(archive->file);
	free(archive);

	return LSARF_OK;
}

int libsarf_read_file_header(libsarf_archive_t* archive, libsarf_file_t* file_header) {
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

		// deallocating memery here appends a kind of number at the filename
		// - interesting language C is -
	}
	else {
		file_header = NULL;
		return LSARF_ERR_A_CANNOT_OPEN;
	}

	return LSARF_OK;
}

int libsarf_add_file(libsarf_archive_t* archive, const char* target, const char* destination) {
	struct stat target_stat;
	stat(target, &target_stat);

	if (!S_ISREG(target_stat.st_mode)) {
		return LSARF_ERR_NOT_REG_FILE;
	}

	int target_offset = 0;
	while ((target[target_offset] == '.' && (target[target_offset+1] == '.' || target[target_offset+1] == '/'))
			|| target[target_offset] == '/')
	{
		target_offset++;
	}

	char* target_final = malloc(sizeof(char) * (strlen(target) - target_offset + 1));
	// fix this issue: (I DARE YOU, I DOUBLE DARE YOU MOTHERFUCKER)
	//  before memcpy: target="./some/example/target"
	memcpy(target_final, target + target_offset, strlen(target) - target_offset);
	//  after memcpy:  target="./some/example/target./some/example/target"

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
		return LSARF_ERR_T_FILENAME_MAX;
	}

	FILE* target_file = fopen(target, "rb");
	if (target_file == NULL) {
		return LSARF_ERR_T_CANNOT_OPEN;
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
	char buffer[1024];
	while (!feof(target_file)) {
		size_t bytes_read = fread(buffer, 1, 1024, target_file);
		fwrite(buffer, 1, bytes_read, archive->file);
	}

	fclose(target_file);

	return LSARF_OK;
}

// **handle errors**
// i'm thinking removing this from the library and
// if everybody wants to add a dir he can implement
// his own function (on his code)
int libsarf_add_dir(libsarf_archive_t* archive, const char* target_dir, const char* destination, sarf_flags_t flags) {
	if (destination != NULL && strlen(destination) > 0) {
		struct stat dest_stat;
		stat(destination, &dest_stat);
		if (S_ISDIR(dest_stat.st_mode) && destination[strlen(destination)-1] != '/') {
			return LSARF_ERR_D_INVALID;
		}
	}

	DIR *dir = opendir(target_dir);
	if (dir != NULL) {
		struct dirent *ent;
		while ((ent = readdir(dir)) != NULL) {
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
				continue;

			char* entpath = malloc(sizeof(char) * (strlen(target_dir) + strlen(ent->d_name)));
			sprintf(entpath, "%s/%s", target_dir, ent->d_name);

			struct stat path_stat;
			stat(entpath, &path_stat);

			if (S_ISDIR(path_stat.st_mode) && (flags & LSARF_AR_ADD_DIR_RECURS)) {
				libsarf_add_dir(archive, entpath, destination, flags);
			} else {
				libsarf_add_file(archive, entpath, destination);
			}
		}

		closedir(dir);
	}

	return LSARF_OK;
}

int libsarf_extract_all(libsarf_archive_t* archive, const char* output) {
	return libsarf_extract_file(archive, NULL, output);
}

int libsarf_extract_file(libsarf_archive_t* archive, const char* target, const char* output) {
	int extract_all = target == NULL ? 0 : -1;
	int extract_folder = extract_all != 0 && target[strlen(target) - 1] == '/' ? 0 : -1;

	int file_found = -1;

	fseek(archive->file, 0, SEEK_SET);
	while (ftell(archive->file) < archive->stat.st_size) {
		libsarf_file_t* file_header = malloc(sizeof(libsarf_file_t));
		int res = libsarf_read_file_header(archive, file_header);
		if (res != 0)
			return res;

		if (extract_all != 0 &&
			strcmp(file_header->filename, target) != 0 &&
			(extract_folder && strncmp(file_header->filename, target, strlen(target)) != 0))
		{
			free(file_header);
			fseek(archive->file, file_header->size, SEEK_CUR);
			continue;
		}

		file_found = 0;

		char *final_output = malloc(sizeof(char) * strlen(output) + strlen(file_header->filename) + 1);
		if (extract_all == 0) {
			sprintf(final_output, "%s/%s", output, file_header->filename);
		}
		else {
			if (output == NULL || strlen(output) <= 0)
				strcpy(final_output, file_header->filename);
			else {
				struct stat output_stat;
				stat(output, &output_stat);

				if (S_ISDIR(output_stat.st_mode) || output[strlen(output) - 1] == '/') {
					sprintf(final_output, "%s/%s", output, file_header->filename);
				}
				else
					strcpy(final_output, output);
			}
		}

		char* parent_dirs = dirname(final_output);
		char* current_dir_state = malloc(sizeof(char) * strlen(parent_dirs));
		char* dir_token = strtok(parent_dirs, "/");

		if (final_output[0] == '/')
		 sprintf(current_dir_state, "/%s", dir_token);
		else strcpy(current_dir_state, dir_token);

		while (dir_token != NULL) {
			mkdir(current_dir_state, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			dir_token = strtok(NULL, "/");
			if (dir_token == NULL) break;

			sprintf(current_dir_state, "%s/%s", current_dir_state, dir_token);
		}

		free(current_dir_state);
		free(dir_token);

		FILE* output_file;
		int output_fd = open(final_output, O_WRONLY | O_CREAT, file_header->mode);
		output_file = fdopen(output_fd, "wb");

		if (output_file == NULL) {
			return LSARF_ERR_O_CANNOT_CREATE;
		}

		int64_t bytes_left = file_header->size;
		char buffer[1024];
		while (!feof(archive->file)) {
			size_t read_size = 1024;
			if (bytes_left < 1024) {
				read_size = bytes_left;
			}

			size_t bytes_read = fread(buffer, 1, read_size, archive->file);
			bytes_left -= bytes_read;

			fwrite(buffer, 1, bytes_read, output_file);

			if (bytes_left <= 0)
				break;
		}

		fclose(output_file);
		free(file_header);

		if (extract_all != 0 && extract_folder != 0)
			break;
	}

	if (file_found == -1)
		return LSARF_ERR_TiA_NOT_FOUND;

	return LSARF_OK;
}

int libsarf_count_files(libsarf_archive_t* archive, int* file_count, const char* search) {
	int count_all = search == NULL ? 0 : -1;
	*file_count = 0;

	fseek(archive->file, 0, SEEK_SET);
	while (ftell(archive->file) < archive->stat.st_size) {
		libsarf_file_t* file_header = malloc(sizeof(libsarf_file_t));
		int res = libsarf_read_file_header(archive, file_header);
		if (res != 0)
			return res;

		fseek(archive->file, file_header->size, SEEK_CUR);

		if (count_all == 0 || strncmp(file_header->filename, search, strlen(search)) == 0)
			(*file_count)++;

		free(file_header);
	}

	return LSARF_OK;
}

int libsarf_stat_files(libsarf_archive_t* archive, libsarf_file_t*** stat_files, const char* search) {
	int stat_all = search == NULL ? 0 : -1;
	int file_count = 0;

	fseek(archive->file, 0, SEEK_SET);
	while (ftell(archive->file) < archive->stat.st_size) {
		libsarf_file_t* file_header = malloc(sizeof(libsarf_file_t));
		int res = libsarf_read_file_header(archive, file_header);
		if (res != 0)
			return res;
			
		fseek(archive->file, file_header->size, SEEK_CUR);

		if (stat_all == 0 || strncmp(file_header->filename, search, strlen(search)) == 0) {
			libsarf_file_t* stat = malloc(sizeof(libsarf_file_t));
			stat->filename = strdup(file_header->filename);
			stat->mode = file_header->mode;
			stat->uid = file_header->uid;
			stat->gid = file_header->gid;
			stat->size = file_header->size;
			stat->mod_time = file_header->mod_time;

			(*stat_files)[file_count] = stat;
			file_count++;
		}

		free(file_header);
	}

	return LSARF_OK;
}

int libsarf_remove_file(libsarf_archive_t* archive, const char* target) {
	char *temp_filename = malloc(sizeof(char) * 100);
	sprintf(temp_filename, "%d_tempar.sarf", abs(rand() * 1000));

	FILE* temp_ar = fopen(temp_filename, "wb");
	if (temp_ar == NULL) {
		return LSARF_ERR_TMP_CANNOT_CREATE;
	}

	int file_found = -1;
	fseek(archive->file, 0, SEEK_SET);
	while (ftell(archive->file) < archive->stat.st_size) {
		libsarf_file_t* file_header = malloc(sizeof(libsarf_file_t));
		int res = libsarf_read_file_header(archive, file_header);
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
		char buffer[1024];
		while (!feof(archive->file)) {
			size_t read_size = 1024;
			if (bytes_left < 1024) {
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
		return LSARF_ERR_TiA_NOT_FOUND;
	}

	if (rename(temp_filename, archive->filename) != 0) {
		return LSARF_ERR_O_CANNOT_CREATE;
	}

	return LSARF_OK;
}
