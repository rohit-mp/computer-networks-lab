#include <stdio.h>
#include <unistd.h> // for the read function
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 42000

int main(){
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    char* request = "#DATE";
    send(client_fd, (char*)request, strlen(request), 0);
    printf("Request for date sent\n");

    char buffer[1024] = {0};
    read(client_fd, buffer, 1024);
    printf("Date: %s\n", buffer);

    return 0;
}