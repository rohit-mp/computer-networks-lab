#include <stdio.h>
#include <unistd.h> // for the read function
#include <string.h>
#include <string>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 4216
#define MAXCHAR 1024

using namespace std;

void *readFunc(void *args){
    int* client_fd = (int*)args;
    while(1){
        char recieved[1024];
        int n = read(*client_fd, recieved, MAXCHAR);
        recieved[n] = '\0';
        printf("%s\n", recieved);
    }
}

void *writeFunc(void *args){
    int* client_fd = (int*)args;
    while(1){
        char tosend[MAXCHAR];
        fgets(tosend, MAXCHAR, stdin);
        tosend[strcspn(tosend, "\n")] = '\0';
        write(*client_fd, (char*)tosend, strlen(tosend)); 
        if(strcmp(tosend, "#EXIT") == 0){
            pthread_exit(NULL);
        }
    }
}

int main(){
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    printf("Enter your name: ");
    char name[30];
    fgets(name, 30, stdin);
    name[strcspn(name, "\n")] = '\0';
    write(client_fd, (char*)name, strlen(name));    

    pthread_t readThread, writeThread;
    pthread_create(&writeThread, NULL, writeFunc, (void*)&client_fd);
    pthread_create(&readThread, NULL, readFunc, (void*)&client_fd);

    pthread_join(writeThread, NULL);
    pthread_kill(readThread, SIGKILL);
    close(client_fd);

    return 0;
}