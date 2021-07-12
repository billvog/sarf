#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sarf.h"

int main(int argc, const char *argv[]) {
	if (argc < 3) {
		printf("No enough arguments\n");
		return 1;
	}

	char* archive_file = malloc(sizeof(char) * 64);
	char* target_file = malloc(sizeof(char) * 64);

	strcpy(archive_file, argv[1]);
	strcpy(target_file, argv[2]);

	int sarf_res;

	libsarf_archive* archive = malloc(sizeof(libsarf_archive));

	// Open archive
	sarf_res = libsarf_open_archive(archive, archive_file);
	if (sarf_res != 0) {
		char* error = libsarf_err2str(sarf_res);
		printf("Error opening archive: %s\n", error);
		return sarf_res;
	}

	// Add file
	sarf_res = libsarf_add_file_to_archive(archive, target_file);
	if (sarf_res != 0) {
		char* error = libsarf_err2str(sarf_res);
		printf("Error adding file to archive: %s\n", error);
		return sarf_res;
	}

	return 0;
}