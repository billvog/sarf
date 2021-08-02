#include "utils.h"

void format_mode(char *str, uint16_t mode) {
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

void format_file_size(char *str, int64_t size) {
	double decimal_size;
	char* unit = malloc(sizeof(char) * 6);

	if (size >= 1000000000000) {
		decimal_size = (double) size / 1000000000000.0;
		strcpy(unit, "TB");
	}
	else if (size >= 1000000000) {
		decimal_size = (double) size / 1000000000.0;
		strcpy(unit, "GB");
	}
	else if (size >= 1000000) {
		decimal_size = (double) size / 1000000.0;
		strcpy(unit, "MB");
	}
	else if (size >= 1000) {
		decimal_size = (double) size / 1000.0;
		strcpy(unit, "KB");
	}
	else {
		decimal_size = (double) size;
		strcpy(unit, "B");
	}

	int rounded_size = round(decimal_size);
	sprintf(str, "%d%s", rounded_size, unit);
}

void format_epoch(char *str, long timestamp) {
	struct tm ts;
	ts = *localtime(&timestamp);
	strftime(str, 24, "%d %b %I:%M", &ts);
}

void format_uid(char *str, uint16_t uid) {
	struct passwd *pws = getpwuid(uid);
	if (pws == NULL)
		strcpy(str, "(unknown)");
	else
		strcpy(str, pws->pw_name);
}

void format_gid(char *str, uint16_t gid) {
	struct group *grp = getgrgid(gid);
	if (grp == NULL)
		strcpy(str, "(unknown)");
	else
		strcpy(str, grp->gr_name);
}
