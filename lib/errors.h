#ifndef __SARF_ERRORS__
#define __SARF_ERRORS__

#include <stdlib.h>
#include <string.h>

#define LSARF_OK 				0
#define LSARF_A_CANNOT_OPEN		1
#define LSARF_T_CANNOT_OPEN		2
#define LSARF_NOT_REG_FILE	 	3
#define LSARF_O_CANNOT_CREATE	4
#define LSARF_TiA_NOT_FOUND		5

char* libsarf_err2str(int err);

#endif