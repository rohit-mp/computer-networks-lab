#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctime>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 42000
#define MAXCHAR 1024

int main(){
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));

    listen(server_fd, 1);

    int addrsize = sizeof(address);
    int client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrsize);

    char buffer[1024];
    int n = read(client_fd, buffer, MAXCHAR);
    buffer[n] = '\0';

    if(strcmp(buffer, "#DATE") == 0){
        printf("Recieved request for date\n");
        time_t now = time(0);
        char* str_time = ctime(&now);
        send(client_fd, str_time, strlen(str_time), 0);
    }
    else{
        printf("Recieved invalid request\n");
        char* str = "Sorry, what?";
        send(client_fd, str, strlen(str), 0);
    }

    return 0;
}