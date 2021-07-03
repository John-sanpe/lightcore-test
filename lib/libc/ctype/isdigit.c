/*
 * libc/ctype/isdigit.c
 */

#include <ctype.h>
#include <export.h>

int isdigit(int c)
{
	return ((unsigned)c - '0') < 10;
}
EXPORT_SYMBOL(isdigit);