#ifndef SARF_UTILS
#define SARF_UTILS

#include <sys/stat.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <pwd.h>
#include <grp.h>

void libsarf_format_mode(char *str, uint16_t mode);
void libsarf_format_file_size(char *str, int64_t size);
void libsarf_format_epoch(char *str, long timestamp);
void libsarf_format_uid(char *str, uint16_t uid);
void libsarf_format_gid(char *str, uint16_t gid);

#endif
