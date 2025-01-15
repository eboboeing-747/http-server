#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "./file.h"

#define BUFFER_SIZE 4096

int count_delims(const char* str, const char* delims)
{
    int amount = 0;

    for (int i = 0; str[i] != '\0'; i++)
    {
        for (int j = 0; delims[j] != '\0'; j++)
        {
            if (str[i] == delims[j])
            {
                amount++;
                break;
            }
        }
    }

    return amount;
}

void split(char* str, const char* delims, char** tokens)
{
    char* token = strtok(str, delims);
    tokens[0] = token;
    int i = 1;

    while (token != NULL)
    {
        token = strtok(NULL, delims);
        if (token != NULL)
        {
            tokens[i] = token;
            i++;
        }
    }
}

int main()
{
    int attempt = 0;
    const int PORT = 8080;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address;
    int addr_len = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);
    
    if (server_fd < 0)
    {
        perror("failed to create socket");
        return 0;
    }

    if (bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0)
    {
        perror("failed to bind");
        return 0;
    }

    if (listen(server_fd, 10) < 0)
    {
        perror("failed to listen");
        return 0;
    }

    printf("--waiting for the connection--\n");

    while (true)
    {
        int new_socket = accept(server_fd, (struct sockaddr*) &address, (socklen_t*) &addr_len);

        if (new_socket < 0)
        {
            perror("failed to acquire socket");
            continue;
        }

        attempt++;
        printf("new connection attempt: %d\n", attempt);

        char buffer[BUFFER_SIZE] = { 0 };
        int valread = read(new_socket, buffer, BUFFER_SIZE);

        if (valread < 0)
        {
            printf("no data to read\n");
        }

        printf("\n--request--\n\n%s\n^^request^^\n", buffer);

        char* delims = " \n";
        // int size = count_delims(buffer, delims) + 1;
        // printf("delims counted: %s\n", size);
        char** tokens = (char**)malloc(sizeof(char*) * 100);
        split(buffer, delims, tokens);
        printf("split done\n");

        for (int i = 0; i < 100; i++)
        {
            printf("%d: %s\n", i, tokens[i]);
        }

        char* file_path = tokens[1];

        if (!strcmp(file_path, "/"))
            file_path = "index.html";

        FILE* file = fopen(file_path, "r");

        if (file == NULL)
        {
            printf("failed to open '%s'\n", file_path);
            continue;
        }

        int filesize = file_size(file);
        char* contents = (char*)malloc(sizeof(char) * filesize);
        bool is_valid = read_file(file, contents, filesize, filesize);

        char* header = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: ";

        int content_length = snprintf(NULL, 0, "%d", filesize) + 1;
        char* content_length_str = (char*)malloc(sizeof(char) * content_length);
        snprintf(content_length_str, content_length, "%d", filesize);

        char* response = (char*)malloc(sizeof(char) * (strlen(header) + content_length + filesize + 2));
        int seer = 0;
        strncpy(response, header, strlen(header));
        seer += strlen(header);
        strncpy(response + seer, content_length_str, strlen(content_length_str));
        seer += strlen(content_length_str);
        response[seer] = '\n';
        response[seer + 1] = '\n';
        seer += 2;
        strncpy(response + seer, contents, filesize);

        write(new_socket, response, strlen(response));
        printf("--response sent--\n--%s--\n", file_path);

        free(tokens);
        free(contents);
        free(content_length_str);
        free(response);
        close(new_socket);
    }

    return 0;
}
