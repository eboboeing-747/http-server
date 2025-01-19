#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "parse.h"

// debug libs
#include <stdio.h>

bool is_scheme(const char* str)
{
    const char* i = str;
    while (*i && isalpha(*i))
        i++;

    return (i > str) && (strncmp(i, "://", 3) == 0);
}

size_t count_delims(const char* str, const char* delimeter)
{
    size_t amount = 0;
    size_t delim_size = strlen(delimeter);

    for (const char* i = str; (i = strstr(i, delimeter)); i += delim_size)
        amount++;

    return amount;
}

void free(struct str_arr* str_array)
{
    free(str_array.array);
}

char* split_next(char* str, const char* delimeter)
{
    if (!*str)
        return NULL;

    char* end = strstr(str, delimeter);

    if (end == NULL)
        return NULL;

    *end = '\0';

    return end + strlen(delimeter);
}

struct str_arr split(char* str, const char* delimeter)
{
    struct str_arr tokens;
    tokens.size = count_delims(str, delimeter) + 1;
    tokens.array = (char**)malloc(sizeof(char*) * tokens.size);

    for (size_t i = 0; str != NULL; str = split_next(str, delimeter), i++)
        tokens.array[i] = str;

    return tokens;
}

char* split_request(char* request, enum status_code* status)
{
    const char* request_delimeter = "\r\n\r\n";
    char* message_body = strstr(request, request_delimeter);

    if (message_body == NULL)
    {
        *status = CE_400; // no "\r\n\r\n"
        return NULL;
    }

    *message_body = '\0';
    message_body += strlen(request_delimeter);

    *status = VALID;
    return message_body;
}

enum status_code validate_http_method(char* method)
{
    for (int i = 0; i < HTTP_METHOD_AMOUNT; i++)
    {
        if (strcmp(method, HTTP_METHODS[i]) == 0)
            return VALID;
    }

    return CE_400;
}

struct request_uri validate_http_uri(char* uri_str, char* method, enum status_code* status)
{
    struct request_uri uri;
    uri.uri = uri_str;
    *status = VALID;

    if (!*uri_str)
    {
        *status = CE_400; // request-URI can never be empty
        uri.type = INVALID;
        return uri;
    }

    if (strcmp(uri_str, "*") == 0)
    {
        if (strcmp(method, "OPTIONS") == 0)
        {
            uri.type = asterisk;
            return uri;
        }
        else
        {
            *status = CE_405; // the only method campatible with request-URI = "*" is OPTIONS
            uri.type = asterisk;
            return uri;
        }
    }

    if (is_scheme(uri_str))
    {
        uri.type = absoluteURI; // if request-URI starts with scheme
        return uri;
    }

    if (*uri_str == '/')
    {
        uri.type = abs_path; // if request-URI starts with "/"
        return uri;
    }
    
    // TODO: validate uri_str as authority
    uri.type = authority;
    if (strcmp(method, "OPTIONS") != 0)
    {
        *status = CE_405; // the only method campatible with request-URI = authority is OPTIONS
        return uri;
    }

    return uri;
}

int vercmp(const struct http_version* lho, const struct http_version* rho)
{
    if (lho->major < rho->major)
        return -1;
    else if (lho->major > rho->major)
        return 1;

    if (lho->minor < rho->minor)
        return -1;
    else if (lho->minor > rho->minor)
        return 1;
    else
        return 0;
}

bool isnumber(const char* number)
{
    if (!*number || number == NULL)
        return false;

    for (; *number; number++)
    {
        if (!isdigit(*number))
            return false;
    }

    return true;
}

struct http_version parse_http_version(char* version_str, enum status_code* status)
{
    struct http_version version;
    version.major = HTTP_MAJOR;
    version.minor = HTTP_MINOR;
    *status = CE_400;

    char* major_str = split_next(version_str, "/");
    if (major_str == NULL)
        return version;

    char* minor_str = split_next(major_str, ".");
    if (minor_str == NULL)
        return version;

    if (!isnumber(major_str) || !isnumber(minor_str))
        return version;

    version.major = atoi(major_str);
    version.minor = atoi(minor_str);
    *status = VALID;
    return version;
}

struct request_line parse_request_line(struct str_arr* tokens, enum status_code* status)
{
    char* request_line_str = tokens->array[0];
    struct str_arr request_line_tokens = split(request_line_str, " ");
    struct request_line req_line;

    if (request_line_tokens.size != REQ_LINE_SIZE)
    {
        *status = CE_400; // 400 Bad Request (invalid request line length)
        free(*request_line_tokens);
        return req_line;
    }

    *status = validate_http_method(request_line_tokens.array[0]);
    if (status != VALID)
    {
        free(*request_line_tokens);
        return req_line;
    }
    req_line.method = request_line_tokens.array[0];

    struct request_uri uri = validate_http_uri(request_line_tokens.array[1], req_line.method, status);
    if (*status != VALID)
    {
        free(*request_line_tokens);
        return req_line;
    }
    req_line.uri = uri;

    struct http_version version = parse_http_version(request_line_tokens.array[2], status);
    if (*status != VALID)
    {
        free(*request_line_tokens);
        return req_line;
    }
    req_line.version = version;

    free(&request_line_tokens);
    *status = VALID;
    return req_line;
}

enum status_code parse_request(char* request)
{
    enum status_code status;

    char* message_body = split_request(request, &status);
    if (status != VALID)
        return CE_400;

    struct str_arr tokens = split(request, "\r\n");

    if (tokens.size < 1)
    {
        free(&tokens);
        return CE_400; // 400 Bad Request (no header fields)
    }

    struct request_line = parse_request_line(tokens, &status);
    if (status != VALID)
    {
        free(&tokens);
        return status;
    }

    printf("method: '%s'\nrequest-URI: '%s', type: '%d'\nHTTP-version/'%d'.'%d'\n\n", req_line.method, req_line.uri.uri, req_line.uri.type, req_line.version.major, req_line.version.minor);

    free(&tokens);
    return VALID;
}

int main()
{
    char request[] = "GET /style.css HTTP/1.1\r\nHost: localhost:8080\r\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:128.0) Gecko/20100101 Firefox/128.0\r\nAccept: text/css,*/*;q=0.1\r\nAccept-Language: en-US,en;q=0.5\r\nAccept-Encoding: gzip, deflate, br, zstd\r\nConnection: keep-alive\r\nReferer: http://localhost:8080/\r\nSec-Fetch-Dest: style\r\nSec-Fetch-Mode: no-cors\r\nSec-Fetch-Site: same-origin\r\nPriority: u=2\r\n\r\n";

    enum status_code status = parse_request(request);
    printf("%d", status);

    return 0;
}
