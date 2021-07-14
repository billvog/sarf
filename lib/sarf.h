/*
	sarf.h includes all include files required from the sarf library
*/

#ifndef __SARF_LIB__
#define __SARF_LIB__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <libgen.h>

#define LSARF_VERSION "0.0.1"

#include "archive.h"
#include "errors.h"
#include "utils.h"
#include "sarf_limits.h"

int libsarf_init();

#endif