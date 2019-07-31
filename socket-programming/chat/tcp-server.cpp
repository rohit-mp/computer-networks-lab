#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctime>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 4210
#define MAXCHAR 1024

void *readFunc(void *args){
    int* client_fd = (int*)args;
    while(1){
        char recieved[MAXCHAR];
        int n = read(*client_fd, recieved, MAXCHAR);
        recieved[n] = '\0';
        if(strcmp(recieved, "#EXIT") == 0){
            pthread_exit(NULL);
        }
        printf("Client: %s\n", recieved);
    }
}

void *writeFunc(void *args){
    int* client_fd = (int*)args;
    while(1){
        char tosend[MAXCHAR];
        fgets(tosend, MAXCHAR, stdin);
        write(*client_fd, (char*)tosend, strlen(tosend));
    }
}

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

    pthread_t readThread, writeThread;
    pthread_create(&readThread, NULL, readFunc, (void*)&client_fd);
    pthread_create(&writeThread, NULL, writeFunc, (void*)&client_fd);

    pthread_join(readThread, NULL);
    pthread_kill(writeThread, SIGKILL);
    close(server_fd);

    return 0;
}