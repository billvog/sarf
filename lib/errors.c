#include "errors.h"

const char* libsarf_err2str(int err) {
	char* error_str = malloc(sizeof(char) * 100);

	switch (err) {
		case LSARF_ERR_A_CANNOT_OPEN:
			strcpy(error_str, "couldn't open archive file");
			break;
		case LSARF_ERR_T_CANNOT_OPEN:
			strcpy(error_str, "couldn't open target file");
			break;
		case LSARF_ERR_NOT_REG_FILE:
			strcpy(error_str, "not regural file");
			break;
		case LSARF_ERR_O_CANNOT_CREATE:
			strcpy(error_str, "couldn't create output file");
			break;
		case LSARF_ERR_TiA_NOT_FOUND:
			strcpy(error_str, "target not found in archive");
			break;
		case LSARF_ERR_TMP_CANNOT_CREATE:
			strcpy(error_str, "couldn't create temp file");
			break;
		case LSARF_ERR_D_INVALID:
			strcpy(error_str, "invalid destination path given");
			break;
		case LSARF_ERR_T_FILENAME_MAX:
			strcpy(error_str, "target filename has exceeded the maximum path length");
			break;
		case LSARF_OK:
			error_str = NULL;
			break;
		default:
			strcpy(error_str, "unknown error occured");
			break;
	}

	return error_str;
}
