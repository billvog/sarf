#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sarf.h"

int print_help() {
	printf("sarf, a simple archive format for unix\n\n");
	printf("Usage: sarf [ARCHIVE] [SWITCHES...] [FILE FROM ARCHIVE]\n\n");
	printf("--help\t\t\tDisplay this help and exit\n");
	printf("--version\t\tDisplay version information and exit\n");
	printf("\n");
	printf("Switches\n\n");
	printf("--add [file] [dest in archive]		Add file to archive\n");
	printf("--remove [file]						Remove file from archive\n");
	printf("--list								List files in archive\n");
	printf("--extract							Extract from archive\n");
	printf("--print								Print file from archive\n");
	printf("\n");
	printf("Find open-source @ https://github.com/billvog/sar-format\n");
	return 0;
}

int main(int argc, const char *argv[]) {
	if (argc == 1) {
		return print_help();
	}

	char* archive_file = malloc(sizeof(char) * 64);
	char* target_file = malloc(sizeof(char) * 64);
	char* target_dest = malloc(sizeof(char) * 64);

	strcpy(archive_file, argv[1]);
	strcpy(target_file, argv[2]);
	strcpy(target_dest, argv[3]);

	int sarf_res;

	libsarf_archive* archive = malloc(sizeof(libsarf_archive));

	// Open archive
	sarf_res = libsarf_open_archive(archive, archive_file);
	if (sarf_res != 0) {
		printf("Error: %s\n", libsarf_err2str(sarf_res));
		return sarf_res;
	}

	// Add file
	sarf_res = libsarf_add_file_to_archive(archive, target_file);
	if (sarf_res != 0) {
		printf("Error: %s\n", libsarf_err2str(sarf_res));
		return sarf_res;
	}

	// Extract file
	sarf_res = libsarf_extract_file_from_archive(archive, target_file, target_dest);
	if (sarf_res != 0) {
		printf("Error: %s\n", libsarf_err2str(sarf_res));
		return sarf_res;
	}

	return 0;
}