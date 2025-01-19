#ifndef PARSE_H
#define PARSE_H

#include <stdlib.h>
#include <stdbool.h>

struct str_arr
{
    char** array;
    size_t size;
};

void free(struct str_arr* str_array);

#define HTTP_METHOD_AMOUNT 8
const char* HTTP_METHODS[HTTP_METHOD_AMOUNT] = { "OPTIONS", "GET", "HEAD", "POST", "PUT", "DELETE", "TRACE", "CONNECT" };

enum status_code
{
    I_100 = 0,
    I_101 = 1,
    S_200 = 2,
    S_201 = 3,
    S_202 = 4,
    S_203 = 5,
    S_204 = 6,
    S_205 = 7,
    S_206 = 8,
    R_300 = 9,
    R_301 = 10,
    R_302 = 11,
    R_303 = 12,
    R_304 = 13,
    R_305 = 14,
    R_306 = 15,
    R_307 = 16,
    CE_400 = 17,
    CE_401 = 18,
    CE_402 = 19,
    CE_403 = 20,
    CE_404 = 21,
    CE_405 = 22,
    CE_406 = 23,
    CE_407 = 24,
    CE_408 = 25,
    CE_409 = 26,
    CE_410 = 27,
    CE_411 = 28,
    CE_412 = 29,
    CE_413 = 30,
    CE_414 = 31,
    CE_415 = 32,
    CE_416 = 33,
    CE_417 = 34,
    SE_500 = 35,
    SE_501 = 36,
    SE_502 = 37,
    SE_503 = 38,
    SE_504 = 39,
    SE_505 = 40,
    VALID = 41
};

enum uri_type
{
    asterisk = 0,
    absoluteURI = 1,
    abs_path = 2,
    authority = 3,
    INVALID = 4
};

struct request_uri
{
    char* uri;
    enum uri_type type;
};

#define HTTP_MAJOR 1
#define HTTP_MINOR 1
struct http_version
{
    int major;
    int minor;
};

int vercmp(const struct http_version* lho, const struct http_version* rho);
// returns: -1 if lho < rho, 1 if lho > rho, 0 if lho == rho

enum status_code validate_http_method(char* method);
// returns: CE_400 if it fails to recognize method

struct request_uri validate_http_uri(char* uri_str, char* method, enum status_code* status);
// makes status be: CE_400 if it

struct http_version parse_http_version(char* version, enum status_code* status);
// makes status be: CE_400 if it fails to recognize version as http-version, SE_505 if http-version not supported, VALID otherwise

#define REQ_LINE_SIZE 3
struct request_line
{
    char* method;
    struct request_uri uri;
    struct http_version version;
};

struct header_field
{
    char* name;
    char* value;
}

char* split_request(char* request);
// returns char* to message-body, makes "\r\n\r\n" -> "\0\n\r\n"

#endif // PARSE_H
