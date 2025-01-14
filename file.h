#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdbool.h>

int file_size(FILE* file);

bool read_file(FILE* file, char* output, int file_size, int output_size);

#endif
