#ifndef __SARF_ERRORS__
#define __SARF_ERRORS__

#include <stdlib.h>
#include <string.h>

#define LSARF_OK 				0
#define LSARF_A_CANNOT_OPEN		1
#define LSARF_NOT_REG_FILE	 	1

char* libsarf_err2str(int err);

#endif