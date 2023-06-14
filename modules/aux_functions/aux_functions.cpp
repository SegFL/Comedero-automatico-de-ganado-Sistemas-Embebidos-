#include "aux_functions.h"
#include "string.h"
#include "mbed.h"

char *strndup(const char *s, size_t n)
{
	size_t l = strlen(s)>n? strlen(s):n;
	char *d = (char*)malloc(l+1);
	if (!d) return NULL;
	memcpy(d, s, l);
	d[l] = 0;
	return d;
}