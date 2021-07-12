#include "errors.h"

char* libsarf_err2str(int err) {
	char* error_str = malloc(sizeof(char) * 100);

	if (err == LSARF_A_CANNOT_OPEN)
		strcpy(error_str, "File cannot be opened");
	else if (err == LSARF_OK)
		strcpy(error_str, "");
	else
		strcpy(error_str, "Unknown error occured");

	return error_str;
}