/*
	This is a simple implemantation of libsarf, used to test
	the library during development.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "sarf.h"
#include "utils.h"

int print_help() {
	printf("sarf, a simple archiving tool for unix.\n");
	printf("Find open-source @ https://github.com/billvog/sarf\n\n");
	printf("Usage: sarf [--help] [--version]\n");
	printf("            [command] [archive] [switches...] [files...]\n\n");
	printf("Commands:\n");
	printf("  -a Add  -x Extract  -l List\n\n");
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

		int ok;
		ok = libsarf_open(archive, archive_file, LSARF_CREATE | LSARF_WRITE);
		if (ok != LSARF_OK) {
			printf("E: %s\n", libsarf_err2str(ok));
			exit(1);
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

				char *target_file = strdup(argv[args_offset + 3]);

				struct stat target_stat;
				stat(target_file, &target_stat);

				FILE *fp = fopen(target_file, "rb");
				if (fp == NULL) {
					printf("E: Cannot open: %s\n", strerror(errno));
					exit(1);
				}

				libsarf_entry_t *entry = malloc(sizeof(libsarf_entry_t));
				libsarf_entry_from_stat(entry, target_stat);
				libsarf_entry_set_name(entry, target_file);

				ok = libsarf_write_entry(archive, entry);
				if (ok != LSARF_OK) {
					printf("E: %s: %s\n", target_file, libsarf_errorstr(archive));
					exit(1);
				}

				char buffer[entry->size];
				fread(buffer, entry->size, 1, fp);
				libsarf_write(archive, buffer, entry->size);

				fclose(fp);
				libsarf_free_entry(entry);

				printf("a %s\n", target_file);
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
							libsarf_close(archive);
							exit(1);
						}
					}
				}
				else {
					if (getcwd(target_dest, sizeof(target_dest)) == NULL) {
						printf("E: cannot get current directory\n");
						libsarf_close(archive);
						exit(1);
					}
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
							libsarf_close(archive);
							exit(1);
						}
					}
				}

				

				printf("e %s\n", target_dest);
			}
		}
		// stat files from archive
		else if (strcmp(command, "-l") == 0) {
			// int file_count = 0;
			// sarf_res = libsarf_count_entries(archive, &file_count, NULL);
			// if (sarf_res != LSARF_OK) {
			// 	printf("E: %s\n", libsarf_err2str(archive->error));
			// 	return 1;
			// }

			// if (file_count == 0) {
			// 	printf("there are no files in the archive\n");
			// 	libsarf_close(archive);
			// 	return 0;
			// }

			// if (file_count > 1)
			// 	printf("found %d files in archive:\n", file_count);

			// for (int i = 0; i < file_count; i++) {
			// 	libsarf_entry_t* s_file = malloc(sizeof(libsarf_entry_t));
			// 	sarf_res = libsarf_stat(archive, s_file, i);
			// 	if (sarf_res != LSARF_OK) {
			// 		printf("E: %d: %s\n", i, libsarf_err2str(archive->error));
			// 		return 1;
			// 	}

			// 	char *mode_str = malloc(sizeof(char) * 10);
			// 	format_mode(mode_str, s_file->mode);							

			// 	char *uid_str = malloc(sizeof(char) * 128);
			// 	format_uid(uid_str, s_file->uid);

			// 	char *gid_str = malloc(sizeof(char) * 128);
			// 	format_gid(gid_str, s_file->gid);

			// 	char *file_size_str = malloc(sizeof(char) * 12);
			// 	if (s_file->mode & S_IFDIR) sprintf(file_size_str, "0");
			// 	else format_file_size(file_size_str, s_file->size);

			// 	char *file_mod_time_str = malloc(sizeof(char) * 12);
			// 	format_epoch(file_mod_time_str, s_file->mod_time);

			// 	printf("%s\t%s\t%s\t%s\t%s\t%s\n",
			// 		mode_str, uid_str, gid_str, file_size_str,
			// 		file_mod_time_str, s_file->filename);

			// 	free(s_file);
			// }
		}

		libsarf_close(archive);
	}

	return 0;
}