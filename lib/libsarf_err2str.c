#include "sarf_int.h"

const char* libsarf_err2str(int err) {
	char* error_str = malloc(sizeof(char) * 100);
	switch (err) {
		case LSARF_ERR_CANNOT_OPEN:
			strcpy(error_str, "Couldn't open");
			break;
		case LSARF_ERR_CANNOT_CREATE:
			strcpy(error_str, "Couldn't create");
			break;
		case LSARF_ERR_NOT_FOUND:
			strcpy(error_str, "Not found");
			break;
		case LSARF_ERR_FILENAME_MAX:
			sprintf(error_str, "File name has exceeded the maximum path length (%d)", PATH_MAX);
			break;
		case LSARF_ERR_CANNOT_WRITE:
			strcpy(error_str, "Couldn't write");
			break;
		case LSARF_ERR_NOT_EXISTS:
			strcpy(error_str, "Not exists");
			break;
		case LSARF_OK:
			error_str = NULL;
			break;
		default:
			strcpy(error_str, "Unknown error occured");
			break;
	}

	return error_str;
}

const char* libsarf_errorstr(libsarf_archive_t* archive) {
	return libsarf_err2str(archive->error);
}