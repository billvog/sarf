#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sarf.h"

int print_help() {
	printf("sarf, a simple archive format for unix\n\n");
	printf("Usage: sarf [ARCHIVE] [SWITCHES...]\n\n");
	printf("--help\t\t\tDisplay this help and exit\n");
	printf("--version\t\tDisplay version information and exit\n");
	printf("\n");
	printf("Switches:\n");
	printf("    -add [file] [destination]\n");
	printf("        Adds file to archive in desired destination if given\n\n");
	printf("    -rm [files]\n");
	printf("        Removes files from archive\n\n");
	printf("    -stat\n");
	printf("        Stats files from archive\n\n");
	printf("    -extract [-all] [file] [-o] [output]\n");
	printf("        Exports either all or a specific file from archive in the desired ouptup path\n\n");
	printf("Find open-source @ https://github.com/billvog/sar-format\n");
	return 0;
}

int main(int argc, const char *argv[]) {
	if (argc == 1 || (argc == 2 && strcmp(argv[1], "--help") == 0)) {
		return print_help();
	}
	else {
		char* archive_file = malloc(sizeof(char) * 64);
		strcpy(archive_file, argv[1]);

		libsarf_archive* archive = malloc(sizeof(libsarf_archive));

		int sarf_res;
		sarf_res = libsarf_open_archive(archive, archive_file);
		if (sarf_res != LSARF_OK) {
			printf("error: %s\n", libsarf_err2str(sarf_res));
			return sarf_res;
		}

		if (argc > 2) {
			// add file to archive
			if (strcmp(argv[2], "-add") == 0) {
				if (argc > 3) {
					char* target_file = malloc(sizeof(char) * 64);
					strcpy(target_file, argv[3]);

					sarf_res = libsarf_add_file_to_archive(archive, target_file);
					if (sarf_res != LSARF_OK) {
						printf("error: %s\n", libsarf_err2str(sarf_res));
						return sarf_res;
					}

					printf("a %s\n", target_file);
				}
				else {
					printf("error: not enough options\nGet help with `%s --help`\n", argv[0]);
					return 1;
				}
			}
			// extract file from archive
			else if (strcmp(argv[2], "-extract") == 0) {
				if (argc > 3) {
					if (strcmp(argv[3], "-all") == 0) {
						char* target_dest = malloc(sizeof(char) * PATH_MAX);

						if (argc > 4) {
							if (strcmp(argv[4], "-o") == 0) {
								if (argc > 5) {
									strcpy(target_dest, argv[5]);
								}
								else {
									printf("error: please specify an output\n");
									libsarf_close_archive(archive);
									exit(1);
								}
							}
						}
						else {
							if (getcwd(target_dest, sizeof(target_dest)) == NULL) {
								printf("error: cannot get current directory\n");
								libsarf_close_archive(archive);
								exit(1);
							}
						}

						sarf_res = libsarf_extract_all_from_archive(archive, target_dest);
						if (sarf_res != LSARF_OK) {
							printf("Error: %s\n", libsarf_err2str(sarf_res));
							return sarf_res;
						}

						printf("e *\n");
					}
					else {
						char* target_file = malloc(sizeof(char) * 100);
						char* target_dest = malloc(sizeof(char) * 100);

						strcpy(target_file, argv[3]);

						if (argc > 4) {
							if (strcmp(argv[4], "-o") == 0) {
								if (argc > 5) {
									strcpy(target_dest, argv[5]);
								}
								else {
									printf("error: please specify an output\n");
									libsarf_close_archive(archive);
									exit(1);
								}
							}
						}
						else {
							strcpy(target_dest, target_file);
						}

						sarf_res = libsarf_extract_file_from_archive(archive, target_file, target_dest);
						if (sarf_res != LSARF_OK) {
							printf("Error: %s\n", libsarf_err2str(sarf_res));
							return sarf_res;
						}

						printf("e %s\n", target_file);
					}
				}
				else {
					printf("error: not enough options\nGet help with `%s --help`\n", argv[0]);
					return 1;
				}
			}
			// remove file from archive
			else if (strcmp(argv[2], "-rm") == 0) {
				if (argc > 3) {
					char* target_file = malloc(sizeof(char) * 64);
					strcpy(target_file, argv[3]);

					sarf_res = libsarf_remove_file_from_archive(archive, target_file);
					if (sarf_res != LSARF_OK) {
						printf("Error: %s\n", libsarf_err2str(sarf_res));
						return sarf_res;
					}

					printf("rm %s\n", target_file);
				}
				else {
					printf("error: not enough options\nGet help with `%s --help`\n", argv[0]);
					return 1;
				}
			}
			// stat files from archive
			else if (strcmp(argv[2], "-stat") == 0) {
				int file_count = 0;
				sarf_res = libsarf_count_files_in_archive(archive, &file_count);
				if (sarf_res != LSARF_OK) {
					printf("error: %s\n", libsarf_err2str(archive->error));
					return 1;
				}

				libsarf_file** stat_files = malloc(sizeof(libsarf_file **) * file_count);
				sarf_res = libsarf_stat_files_from_archive(archive, &stat_files);
				if (sarf_res != LSARF_OK) {
					printf("error: %s\n", libsarf_err2str(archive->error));
					return 1;
				}

				printf("Found %d file(s) in archive:\n", file_count);
				for (int i = 0; i < file_count; i++) {
					libsarf_file* s_file = stat_files[i];

					char *mode_str = malloc(sizeof(char) * 10);
					libsarf_format_mode(mode_str, s_file->mode);							

					char *uid_str = malloc(sizeof(char) * 128);
					libsarf_format_uid(uid_str, s_file->uid);

					char *gid_str = malloc(sizeof(char) * 128);
					libsarf_format_gid(gid_str, s_file->gid);

					char *file_size_str = malloc(sizeof(char) * 12);
					libsarf_format_file_size(file_size_str, s_file->size);

					char *file_mod_time_str = malloc(sizeof(char) * 12);
					libsarf_format_epoch(file_mod_time_str, s_file->mod_time);

					printf("%s\t%s\t%s\t%s\t%s\t%s\n",
						mode_str, uid_str, gid_str, file_size_str,
						file_mod_time_str, s_file->filename);
				}
			}
		}

		libsarf_close_archive(archive);
	}

	return 0;
}