#ifndef __SARF_UTILS__
#define __SARF_UTILS__

#include <time.h>
#include <pwd.h>
#include <grp.h>

void libsarf_format_mode(char *str, uint16_t mode);
void libsarf_format_file_size(char *str, int64_t size);
void libsarf_format_epoch(char *str, long timestamp);
void libsarf_format_uid(char *str, uint16_t uid);
void libsarf_format_gid(char *str, uint16_t gid);

#endif