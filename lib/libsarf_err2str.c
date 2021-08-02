#include "sarf_int.h"

const char* libsarf_err2str(int err) {
	char* error_str = malloc(sizeof(char) * 100);
	switch (err) {
		case LSARF_ERR_A_CANNOT_OPEN:
			strcpy(error_str, "Couldn't open archive file");
			break;
		case LSARF_ERR_T_CANNOT_OPEN:
			strcpy(error_str, "Couldn't open target file");
			break;
		case LSARF_ERR_NOT_REG_FILE:
			strcpy(error_str, "Not regural file");
			break;
		case LSARF_ERR_O_CANNOT_CREATE:
			strcpy(error_str, "Couldn't create output file");
			break;
		case LSARF_ERR_TiA_NOT_FOUND:
			strcpy(error_str, "Target not found in archive");
			break;
		case LSARF_ERR_TMP_CANNOT_CREATE:
			strcpy(error_str, "Couldn't create temp file");
			break;
		case LSARF_ERR_D_INVALID:
			strcpy(error_str, "Invalid destination path given");
			break;
		case LSARF_ERR_T_FILENAME_MAX:
			strcpy(error_str, "Target filename has exceeded the maximum path length");
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
