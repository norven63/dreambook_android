#pragma once

#ifdef _WIN32_WCE

#include <time.h>

tm * localtime(const time_t *timer);
struct tm *gmtime(const time_t *t);
time_t mktime(struct tm* pt);
time_t time( time_t *timer );
size_t strftime(char *s, size_t maxs, const char *f, const struct tm *t);

struct stat {
	unsigned int     st_dev;
	unsigned short     st_ino;
	unsigned short st_mode;
	short      st_nlink;
	short      st_uid;
	short      st_gid;
	unsigned int  st_rdev;
	long     st_size;
	time_t st_atime;
	time_t st_mtime;
	time_t st_ctime;
};

int stat(const char *path, struct stat *buffer);
int remove(const char *path);
int rename(const char * _OldFilename, const char * _NewFilename);

#endif