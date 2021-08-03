#include "sarf_int.h"

int libsarf_add_dir(libsarf_archive_t* archive, const char* target_dir, const char* destination, sarf_flags_t flags) {
	if (archive->open_mode == LSARF_READ_ONLY)
		return LSARF_ERR_CANNOT_WRITE;


	// if (destination != NULL && strlen(destination) > 0) {
	// 	struct stat dest_stat;
	// 	stat(destination, &dest_stat);
	// 	if (S_ISDIR(dest_stat.st_mode) && destination[strlen(destination)-1] != '/') {
	// 		return LSARF_ERR_D_INVALID;
	// 	}
	// }

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
			
			libsarf_add_file(archive, entpath, destination);

			if (S_ISDIR(path_stat.st_mode) && (flags & LSARF_RECURSIVE)) {
				libsarf_add_dir(archive, entpath, destination, flags);
			}
		}

		closedir(dir);
	}

	return LSARF_OK;
}