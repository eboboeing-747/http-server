#ifndef FILE_C
#define FILE_C

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "file.h"

int file_size(FILE* file)
{
    fseek(file, 0L, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    return file_size;
}

bool read_file(FILE* file, char* output, int file_size, int output_size)
{
    if (output_size < file_size)
        return false;

    for (int i = 0; i < file_size; i++)
        output[i] = fgetc(file);

    return true;
}

#endif // FILE_C
