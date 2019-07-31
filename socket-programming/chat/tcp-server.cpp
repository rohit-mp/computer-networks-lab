#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctime>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 4208
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

    while(1){
        char recieved[MAXCHAR];
        int n = read(client_fd, recieved, MAXCHAR);
        recieved[n] = '\0';
        printf("Client: %s\n", recieved);

        char tosend[MAXCHAR];
        fgets(tosend, MAXCHAR, stdin);
        write(client_fd, (char*)tosend, strlen(tosend));
    }

    return 0;
}