#include "utils.h"

void libsarf_format_mode(char *str, uint16_t mode) {
	str[0] = (mode & S_IFDIR) ? 'd' : '-';
	str[1] = (mode & S_IRUSR) ? 'r' : '-';
	str[2] = (mode & S_IWUSR) ? 'w' : '-';
	str[3] = (mode & S_IXUSR) ? 'x' : '-';
	str[4] = (mode & S_IRGRP) ? 'r' : '-';
	str[5] = (mode & S_IWGRP) ? 'w' : '-';
	str[6] = (mode & S_IXGRP) ? 'x' : '-';
	str[7] = (mode & S_IROTH) ? 'r' : '-';
	str[8] = (mode & S_IWOTH) ? 'w' : '-';
	str[9] = (mode & S_IXOTH) ? 'x' : '-';
	str[10] = '\0';
}

void libsarf_format_file_size(char *str, int64_t size) {
	if (size >= 1000000000000)
		sprintf(str, "%lldT", size / 1000000000000);
	else if (size >= 1000000000)
		sprintf(str, "%lldG", size / 1000000000);
	else if (size >= 1000000)
		sprintf(str, "%lldM", size / 1000000);
	else if (size >= 1000)
		sprintf(str, "%lldK", size / 1000);
	else
		sprintf(str, "%lldB", size);
}

void libsarf_format_epoch(char *str, long timestamp) {
	struct tm ts;
	ts = *localtime(&timestamp);
	strftime(str, 24, "%d %b %I:%M", &ts);
}

void libsarf_format_uid(char *str, uint16_t uid) {
	struct passwd *pws = getpwuid(uid);
	if (pws == NULL)
		strcpy(str, "(unknown)");
	else
		strcpy(str, pws->pw_name);
}

void libsarf_format_gid(char *str, uint16_t gid) {
	struct group *grp = getgrgid(gid);
	if (grp == NULL)
		strcpy(str, "(unknown)");
	else
		strcpy(str, grp->gr_name);
}
