/*
	sarf.h includes all include files required from the sarf library
*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SARF_LIB
#define SARF_LIB

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
#include <dirent.h>

#include "archive.h"
#include "errors.h"
#include "utils.h"
#include "flags.h"

#define LSARF_VERSION "0.0.12"

#ifndef LSARF_BUILT_OS
#define LSARF_BUILT_OS "OS"
#endif

int libsarf_init();

#endif

#ifdef __cplusplus
}
#endif