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
	printf("Switches\n\n");
	printf("-add [file] [destination in archive]?\n");
	printf("-remove [file from archive]\n");
	printf("-stat [-all]? [file in archive]?\n");
	printf("-extract [-all]? [file] [output]?\n");
	printf("-cat [file in archive]\n");
	printf("\n");
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
		if (sarf_res != 0) {
			printf("Error: %s\n", libsarf_err2str(sarf_res));
			return sarf_res;
		}

		if (argc > 2) {
			// add file to archive
			if (strcmp(argv[2], "-add") == 0) {
				if (argc > 3) {
					char* target_file = malloc(sizeof(char) * 64);
					strcpy(target_file, argv[3]);

					sarf_res = libsarf_add_file_to_archive(archive, target_file);
					if (sarf_res != 0) {
						printf("Error: %s\n", libsarf_err2str(sarf_res));
						return sarf_res;
					}
				}
				else {
					printf("Error: not enough options\nGet help with `%s --help`", argv[0]);
					return 1;
				}
			}
			// extract file from archive
			else if (strcmp(argv[2], "-extract") == 0) {
				if (argc > 3) {
					char* target_file = malloc(sizeof(char) * 64);
					strcpy(target_file, argv[3]);

					char* target_dest = malloc(sizeof(char) * 64);
					if (argc > 4) {
						strcpy(target_dest, argv[4]);
					}
					else {
						strcpy(target_dest, target_file);
					}

					sarf_res = libsarf_extract_file_from_archive(archive, target_file, target_dest);
					if (sarf_res != 0) {
						printf("Error: %s\n", libsarf_err2str(sarf_res));
						return sarf_res;
					}
				}
				else {
					printf("Error: not enough options\nGet help with `%s --help`", argv[0]);
					return 1;
				}
			}
			// sarf me.sarf -stat -all
			else if (strcmp(argv[2], "-stat") == 0) {
				if (argc > 3) {
					if (strcmp(argv[3], "-all") == 0) {
						int file_count = 0;
						libsarf_stat_file* stat_files[10];
						sarf_res = libsarf_stat_files_from_archive(archive, stat_files, &file_count);
						if (sarf_res != 0) {
							printf("Error: %s\n", libsarf_err2str(sarf_res));
							return sarf_res;
						}

						for (int i = 0; i < file_count; i++) {
							libsarf_stat_file* s_file = stat_files[i];
							if (s_file == NULL) continue;

							printf("%s\t%hd\n", s_file->filename, s_file->size);
						}
					}
				}
				else {
					printf("Error: not enough options\nGet help with `%s --help`", argv[0]);
					return 1;
				}
			}
		}
	}

	return 0;
}