#include "aux_functions.h"
#include "string.h"
#include "mbed.h"

char *strndup(const char *s, size_t n)
{
	size_t l = strlen(s)>n? strlen(s):n;
	char *d = (char*)malloc(l+1);
	if (!d) 
        return NULL;
	memcpy(d, s, l);
	d[l] = 0;
	return d;
}

int _strcmp(const char *s1, const char *s2) {
   while (*s1 != '\0' && *s2 != '\0'  && *s1 == *s2) {
      s1++;
      s2++;
   }
   return *s1 - *s2;
}
