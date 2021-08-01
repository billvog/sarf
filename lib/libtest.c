/*
	This is a simple implemantation of libsarf, used to test
	the library during development.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libsarf.h"

int print_help() {
	printf("sarf, a simple archiving tool for unix.\n");
	printf("Find open-source @ https://github.com/billvog/sarf\n\n");
	printf("Usage: sarf [--help] [--version]\n");
	printf("            [command] [archive] [switches...] [files...]\n\n");
	printf("Commands:\n");
	printf("  -a Add/Replace  -rm Remove  -x Extract  -s Stat\n\n");
	printf("Switches:\n");
	printf("  -d Specifies the destination of a file in archive\n");
	printf("  -o Specifies the output path in case of extracting\n");
	return 0;
}

int main(int argc, const char *argv[]) {
	if (argc == 1 || (argc == 2 && strcmp(argv[1], "--help") == 0)) {
		return print_help();
	}
	else if (strcmp(argv[1], "--version") == 0) {
		printf("sarf version %s (%s)\n", LSARF_VERSION, LSARF_BUILT_OS);
		return 0;
	}
	else {
		if (libsarf_init() != 0) {
			printf("E: unknown error during initialization\n");
			exit(1);
		}

		if (argc < 3) {
			printf("E: please provide an archive file\n");
			exit(1);
		}

		char* command = strdup(argv[1]);

		char* archive_file = malloc(sizeof(char) * 64);
		strcpy(archive_file, argv[2]);

		libsarf_archive_t* archive = malloc(sizeof(libsarf_archive_t));

		int sarf_res;
		sarf_res = libsarf_open_archive(archive, archive_file, LSARF_WRITE);
		if (sarf_res != LSARF_OK) {
			printf("E: %s\n", libsarf_err2str(sarf_res));
			return sarf_res;
		}

		// add file to archive
		if (strcmp(command, "-a") == 0) {
			if (argc > 2) {
				// find destintation if given
				int args_offset = 0;
				char *target_dest = malloc(sizeof(char) * 100);
				if (strcmp(argv[3], "-d") == 0) {
					if (argc > 4) {
						args_offset = 2;
						strcpy(target_dest, argv[4]);
					}
					else {
						printf("E: please specify destination");
						exit(1);
					}
				}

				for (int i = 2 + args_offset; i < argc; ++i) {
					char *target_file = malloc(sizeof(char) * 100);
					strcpy(target_file, argv[i]);

					struct stat target_stat;
					stat(target_file, &target_stat);

					if (S_ISDIR(target_stat.st_mode))
						sarf_res = libsarf_add_dir(archive, target_file, target_dest, LSARF_AR_ADD_DIR_RECURS);
					else
						sarf_res = libsarf_add_file(archive, target_file, target_dest);

					if (sarf_res != LSARF_OK) {
						printf("E: %s: %s\n", target_file, libsarf_err2str(sarf_res));
						continue;
					}

					printf("a %s\n", target_file);
				}
			}
			else {
				printf("E: not enough options\n");
				return 1;
			}
		}
		// extract file from archive
		else if (strcmp(command, "-x") == 0) {
			if (argc == 3 || (argc == 5 && strcmp(argv[3], "-o") == 0)) {
				char* target_dest = malloc(sizeof(char) * PATH_MAX);

				if (argc > 3) {
					if (strcmp(argv[3], "-o") == 0) {
						if (argc > 4) {
							strcpy(target_dest, argv[4]);
						}
						else {
							printf("E: please specify an output\n");
							libsarf_close_archive(archive);
							exit(1);
						}
					}
				}
				else {
					if (getcwd(target_dest, sizeof(target_dest)) == NULL) {
						printf("E: cannot get current directory\n");
						libsarf_close_archive(archive);
						exit(1);
					}
				}

				sarf_res = libsarf_extract_all(archive, target_dest);
				if (sarf_res != LSARF_OK) {
					printf("E: %s\n", libsarf_err2str(sarf_res));
					return sarf_res;
				}

				printf("e *\n");
			}
			else {
				char* target_file = malloc(sizeof(char) * 100);
				char* target_dest = malloc(sizeof(char) * 100);

				strcpy(target_file, argv[3]);

				if (argc > 3) {
					if (strcmp(argv[4], "-o") == 0) {
						if (argc > 4) {
							strcpy(target_dest, argv[4]);
						}
						else {
							printf("E: please specify an output\n");
							libsarf_close_archive(archive);
							exit(1);
						}
					}
				}

				sarf_res = libsarf_extract_file(archive, target_file, target_dest);
				if (sarf_res != LSARF_OK) {
					printf("E: %s\n", libsarf_err2str(sarf_res));
					return sarf_res;
				}

				printf("e %s\n", target_dest);
			}
		}
		// remove file from archive
		else if (strcmp(command, "-rm") == 0) {
			if (argc > 3) {
				for (int i = 3; i < argc; ++i) {
					char* target_file = malloc(sizeof(char) * 100);
					strcpy(target_file, argv[i]);

					sarf_res = libsarf_remove_file(archive, target_file);
					if (sarf_res != LSARF_OK) {
						printf("E: %s: %s\n", target_file, libsarf_err2str(sarf_res));
						continue;
					}

					printf("r %s\n", target_file);
				}
			}
			else {
				printf("E: not enough options\n");
				return 1;
			}
		}
		// stat files from archive
		else if (strcmp(command, "-s") == 0) {
			if (argc > 3) {
				char* search_file = strdup(argv[3]);

				int file_count = 0;
				sarf_res = libsarf_count_files(archive, &file_count, search_file);
				if (sarf_res != LSARF_OK) {
					printf("E: %s\n", libsarf_err2str(archive->error));
					return 1;
				}

				if (file_count == 0) {
					printf("nothing found\n");
					libsarf_close_archive(archive);
					return 0;
				}

				libsarf_entry_t** stat_files = malloc(sizeof(libsarf_entry_t *) * file_count);
				sarf_res = libsarf_stat_files(archive, &stat_files, search_file);
				if (sarf_res != LSARF_OK) {
					printf("E: %s\n", libsarf_err2str(archive->error));
					return 1;
				}

				if (file_count > 1)
					printf("found %d files in archive:\n", file_count);

				for (int i = 0; i < file_count; i++) {
					libsarf_entry_t* s_file = stat_files[i];

					char *mode_str = malloc(sizeof(char) * 10);
					libsarf_format_mode(mode_str, s_file->mode);

					char *uid_str = malloc(sizeof(char) * 128);
					libsarf_format_uid(uid_str, s_file->uid);

					char *gid_str = malloc(sizeof(char) * 128);
					libsarf_format_gid(gid_str, s_file->gid);

					char *file_size_str = malloc(sizeof(char) * 12);
					if (s_file->mode & S_IFDIR) sprintf(file_size_str, "---");
					else libsarf_format_file_size(file_size_str, s_file->size);

					char *file_mod_time_str = malloc(sizeof(char) * 12);
					libsarf_format_epoch(file_mod_time_str, s_file->mod_time);

					printf("%s\t%s\t%s\t%s\t%s\t%s\n",
						mode_str, uid_str, gid_str, file_size_str,
						file_mod_time_str, s_file->filename);
				}

				free(stat_files);
			}
			else {
				int file_count = 0;
				sarf_res = libsarf_count_files(archive, &file_count, NULL);
				if (sarf_res != LSARF_OK) {
					printf("E: %s\n", libsarf_err2str(archive->error));
					return 1;
				}

				if (file_count == 0) {
					printf("there are no files in the archive\n");
					libsarf_close_archive(archive);
					return 0;
				}

				libsarf_entry_t** stat_files = malloc(sizeof(libsarf_entry_t *) * file_count);
				sarf_res = libsarf_stat_files(archive, &stat_files, NULL);
				if (sarf_res != LSARF_OK) {
					printf("E: %s\n", libsarf_err2str(archive->error));
					return 1;
				}

				if (file_count > 1)
					printf("found %d files in archive:\n", file_count);

				for (int i = 0; i < file_count; i++) {
					libsarf_entry_t* s_file = stat_files[i];

					char *mode_str = malloc(sizeof(char) * 10);
					libsarf_format_mode(mode_str, s_file->mode);							

					char *uid_str = malloc(sizeof(char) * 128);
					libsarf_format_uid(uid_str, s_file->uid);

					char *gid_str = malloc(sizeof(char) * 128);
					libsarf_format_gid(gid_str, s_file->gid);

					char *file_size_str = malloc(sizeof(char) * 12);
					if (s_file->mode & S_IFDIR) sprintf(file_size_str, "0");
					else libsarf_format_file_size(file_size_str, s_file->size);

					char *file_mod_time_str = malloc(sizeof(char) * 12);
					libsarf_format_epoch(file_mod_time_str, s_file->mod_time);

					printf("%s\t%s\t%s\t%s\t%s\t%s\n",
						mode_str, uid_str, gid_str, file_size_str,
						file_mod_time_str, s_file->filename);
				}

				free(stat_files);
			}
		}

		libsarf_close_archive(archive);
	}

	return 0;
}