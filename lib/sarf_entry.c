#include "sarf_int.h"

int sarf_entry_from_stat(libsarf_entry_t* file_header, struct stat stat) {
	file_header->size = stat.st_size;
	file_header->mode = stat.st_mode;
	file_header->uid = stat.st_uid;
	file_header->gid = stat.st_gid;
	file_header->mod_time = stat.st_mtimespec.tv_nsec;
	return LSARF_OK;
}

void sarf_free_entry(libsarf_entry_t* entry) {
	free(entry);
}

void sarf_entry_set_name(libsarf_entry_t* entry, const char *name) {
	entry->filename = strdup(name);
}