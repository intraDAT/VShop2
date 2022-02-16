#include <string.h>
#include "win32tools.h"

int strncasecmp(const char *s1, const char *s2, size_t n)
{
  return _strnicmp(s1,s2,n);
}

int strcasecmp(const char *s1, const char *s2)
{
  return _stricmp(s1,s2);
}
