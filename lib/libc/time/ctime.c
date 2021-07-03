/*
 * libc/time/ctime.c
 */

#include <time.h>
#include <export.h>

char * ctime(const time_t * t)
{
	return asctime((const struct tm *)localtime(t));
}
EXPORT_SYMBOL(ctime);