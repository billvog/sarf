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
	printf("            [archive] [command] [switches...] [files...]\n\n");
	printf("Commands:\n");
	printf("  -a Add  -x Extract (all)  -l List\n\n");
	printf("Switches:\n");
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
		if (argc < 2) {
			printf("E: please provide an archive file\n");
			exit(1);
		}
		else if (argc < 3) {
			printf("E: please provide a command\n");
			exit(1);
		}

		char* archive_file = strdup(argv[1]);
		char* command = strdup(argv[2]);

		// add file
		if (strcmp(command, "-a") == 0) {
			if (argc > 2) {
				libsarf_archive_t* archive = malloc(sizeof(libsarf_archive_t));
				int ok = sarf_open(archive, archive_file, LSARF_WRONLY);
				if (ok != LSARF_OK) {
					printf("E: %s\n", sarf_err2str(ok));
					exit(1);
				}

				char *target_file = strdup(argv[3]);

				struct stat target_stat;
				stat(target_file, &target_stat);

				libsarf_entry_t *entry = malloc(sizeof(libsarf_entry_t));

				// Check if it's directory
				if (target_stat.st_mode & S_IFDIR) {
					sarf_entry_from_stat(entry, target_stat);
					sarf_entry_set_name(entry, target_file);

					ok = sarf_write_entry(archive, entry);
					if (ok != LSARF_OK) {
						printf("E: %s: %s\n", target_file, sarf_errorstr(archive));
						exit(1);
					}
				} else {
					FILE *fp = fopen(target_file, "rb");
					if (fp == NULL) {
						printf("E: Cannot open: %s\n", strerror(errno));
						exit(1);
					}

					sarf_entry_from_stat(entry, target_stat);
					sarf_entry_set_name(entry, target_file);

					ok = sarf_write_entry(archive, entry);
					if (ok != LSARF_OK) {
						printf("E: %s: %s\n", target_file, sarf_errorstr(archive));
						exit(1);
					}

					char buffer[entry->size];
					fread(buffer, entry->size, 1, fp);
					sarf_write(archive, buffer, entry->size);

					fclose(fp);
				}

				sarf_free_entry(entry);
				sarf_close(archive);

				printf("a %s\n", target_file);
			}
			else {
				printf("E: not enough options\n");
				return 1;
			}
		}
		// extract all files
		else if (strcmp(command, "-x") == 0) {
			libsarf_archive_t* archive = malloc(sizeof(libsarf_archive_t));
			int ok = sarf_open(archive, archive_file, LSARF_RDONLY);
			if (ok != LSARF_OK) {
				printf("E: %s\n", sarf_err2str(ok));
				exit(1);
			}

			char* output_path = NULL;
			if (argc > 3 && strcmp(argv[3], "-o") == 0) {
				if (argc > 4) {
					output_path = strdup(argv[4]);

					// remove trailling slash
					if (output_path[strlen(output_path) - 1] == '/') {
						output_path[strlen(output_path) - 1] = '\0';
					}
				}
				else {
					printf("E: please provide an output path\n");
					exit(1);
				}
			}

			libsarf_entry_t *entry = malloc(sizeof(libsarf_entry_t));
			while (sarf_read_entry(archive, entry) != LSARF_NOK) {
				char* output_filepath = NULL;
				if (output_path != NULL) {
					output_filepath = malloc(sizeof(char) * (strlen(output_path) + strlen(entry->filename)));
					sprintf(output_filepath, "%s/%s", output_path, entry->filename);
				}

				if (output_filepath[strlen(output_filepath) - 1] == '/') {
					if (mkdir(output_filepath, entry->mode) != 0) {
						if (errno != EEXIST) {
							printf("E: %s -> %s: Cannot create directory: %s\n", entry->filename, output_filepath, strerror(errno));
							exit(1);
						}
					}

					sarf_skip_file_data(archive, entry);
				}
				else {
					int fd = open(output_filepath == NULL ? entry->filename : output_filepath, O_RDWR | O_CREAT, entry->mode);
					FILE *fp = fdopen(fd, "wb");
					if (fp == NULL) {
						printf("E: Cannot open: %s\n", strerror(errno));
						exit(1);
					}

					char buffer[entry->size];
					sarf_read(archive, buffer, entry->size);
					fwrite(buffer, entry->size, 1, fp);

					fclose(fp);
				}

				printf("e %s -> %s\n", entry->filename, output_filepath);
				free(output_filepath);
			}

			sarf_free_entry(entry);
			sarf_close(archive);
			free(output_path);
		}
		// list files
		else if (strcmp(command, "-l") == 0) {
			libsarf_archive_t* archive = malloc(sizeof(libsarf_archive_t));
			int ok = sarf_open(archive, archive_file, LSARF_RDONLY);
			if (ok != LSARF_OK) {
				printf("E: %s\n", sarf_err2str(ok));
				exit(1);
			}

			int entries_count = 0;

			libsarf_entry_t *entry = malloc(sizeof(libsarf_entry_t));
			while (sarf_read_entry(archive, entry) == LSARF_OK) {
				entries_count++;

				char *mode_string = malloc(sizeof(char) * 11);
				strmode(entry->mode, mode_string);

				char *uid_string = malloc(sizeof(char) * 56);
				format_uid(uid_string, entry->uid);

				char *gid_string = malloc(sizeof(char) * 56);
				format_gid(gid_string, entry->gid);

				char *mtime_string = malloc(sizeof(char) * 24);
				format_epoch(mtime_string, entry->mod_time);

				printf("%s\t%s\t%s\t%lld\t%s\t%s\n", mode_string, uid_string, 
						gid_string, entry->size, mtime_string, entry->filename);
				sarf_skip_file_data(archive, entry);
			}

			sarf_free_entry(entry);
			sarf_close(archive);
		}
	}

	return 0;
}