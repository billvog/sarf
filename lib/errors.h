#ifndef SARF_ERRORS
#define SARF_ERRORS

#include <stdlib.h>
#include <string.h>

#define LSARF_OK 						0
#define LSARF_ERR_A_CANNOT_OPEN			1
#define LSARF_ERR_T_CANNOT_OPEN			2
#define LSARF_ERR_NOT_REG_FILE	 		3
#define LSARF_ERR_O_CANNOT_CREATE		4
#define LSARF_ERR_TiA_NOT_FOUND			5
#define LSARF_ERR_TMP_CANNOT_CREATE		6
#define LSARF_ERR_D_INVALID				7
#define LSARF_ERR_T_FILENAME_MAX		8
#define LSARF_ERR_A_CANNOT_WRITE		9

const char* libsarf_err2str(int err);

#endif