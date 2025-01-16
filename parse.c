#include <stdlib.h>
#include <string.h>

#include <stdio.h>

size_t count_delims(const char* str, const char* delimeter)
{
    size_t amount = 0;
    size_t delim_size = strlen(delimeter);

    for (const char* i = str; (i = strstr(i, delimeter)); i += delim_size)
        amount++;

    return amount;
}

char* split_next(char* str, const char* delimeter)
{
    if (*str == '\0')
        return NULL;

    char* end = strstr(str, delimeter);
    
    if (end == NULL)
        return NULL;

    *end = '\0';

    return end + strlen(delimeter);
}

struct str_arr
{
    char** array;
    size_t size;
};

struct str_arr split(char* str, const char* delimeter)
{
    struct str_arr tokens;
    tokens.size = count_delims(str, delimeter) + 1;
    tokens.array = (char**)malloc(sizeof(char*) * tokens.size);

    for (size_t i = 0; str != NULL; str = split_next(str, delimeter), i++)
        tokens.array[i] = str;

    return tokens;
}

int main()
{
    char request[] = "GET /style.css HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:128.0) Gecko/20100101 Firefox/128.0\r\nAccept: text/css,*/*;q=0.1\r\nAccept-Language: en-US,en;q=0.5\r\nAccept-Encoding: gzip, deflate, br, zstd\r\nConnection: keep-alive\r\nReferer: http://localhost:8080/\r\nSec-Fetch-Dest: style\r\nSec-Fetch-Mode: no-cors\r\nSec-Fetch-Site: same-origin\r\nPriority: u=2\r\n\r\n";

    struct str_arr tokens = split(request, "\r\n");

    for (int i = 0; i < tokens.size; i++)
        printf("%d: '%s'\n", i, tokens.array[i]);

    free(tokens.array);

    return 0;
}
