#include "sarf_int.h"

int libsarf_extract_all(libsarf_archive_t* archive, const char* output) {
	return libsarf_extract(archive, NULL, output);
}

int libsarf_extract(libsarf_archive_t* archive, const char* target, const char* output) {
	int extract_all = target == NULL ? 0 : -1;
	int extract_folder = extract_all != 0 && target[strlen(target) - 1] == '/' ? 0 : -1;

	int file_found = -1;

	fseek(archive->file, 0, SEEK_SET);
	while (ftell(archive->file) < archive->stat.st_size) {
		libsarf_entry_t* file_header = malloc(sizeof(libsarf_entry_t));
		int res = _libsarf_read_entry(archive, file_header);
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
			return LSARF_ERR_CANNOT_CREATE;
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
		return LSARF_ERR_NOT_FOUND;

	return LSARF_OK;
}