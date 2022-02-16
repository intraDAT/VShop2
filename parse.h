#ifndef PARSE_H
#define PARSE_H 1

#include <stdio.h>

void SetFieldSeparator(char separator);
int GetField(FILE *in, char *field, int space);
int NextRecord(FILE *in);

#endif /* PARSE_H */

