#ifndef PARSE_H
#define PARSE_H

#include <stdlib.h>

#include "parse.c"

struct str_arr
{
    char* array[];
    size_t size;
}

size_t count_delims(const char* str, const char* delimeter);

char* split_next(char* str, const char* delimeter);

struct str_arr split(char* str, const char* delimeter);

#endif // PARSE_H
