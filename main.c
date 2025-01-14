#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "./file.h"

#define BUFFER_SIZE 1024

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

        FILE* file = fopen("index.html", "r");
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
        
        printf("--response--\n\n%s\n^^response^^\n", response);
        write(new_socket, response, strlen(response));
        
        free(contents);
        free(content_length_str);
        free(response);
        close(new_socket);
    }

    return 0;
}
