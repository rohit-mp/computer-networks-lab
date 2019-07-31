#include <stdio.h>
#include <unistd.h> // for the read function
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 4208
#define MAXCHAR 1024

int main(){
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    while(1){
        char tosend[MAXCHAR];
        fgets(tosend, MAXCHAR, stdin);
        write(client_fd, (char*)tosend, strlen(tosend));        

        char recieved[1024];
        int n = read(client_fd, recieved, MAXCHAR);
        recieved[n] = '\0';
        printf("Server:%s\n", recieved);
    }

    return 0;
}