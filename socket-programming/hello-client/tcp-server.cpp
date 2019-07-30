#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 42000

int main(){
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    // AF_INET => ipv4 protocol; use AF_INET6 for ipv6
    // SOCK_STREAM => TCP (connection oriented)
    // 0 => use IP as the protocol

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));

    listen(server_fd, 5);

    int addrsize = sizeof(address);
    int client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrsize);

    char* str = "hello client";
    send(client_fd, str, strlen(str), 0);
    printf("Hello message sent\n");

    return 0;
}