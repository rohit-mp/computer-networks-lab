#include <stdio.h>
#include <string.h>
#include <vector>
#include <unistd.h>
#include <ctime>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

#define PORT 4212
#define MAXCHAR 1024

vector<int> clients;

void *readFunc(void *args){
    int *client_fd = (int*)args;

    while(1){
        char recieved[MAXCHAR];
        int n = read(*client_fd, recieved, MAXCHAR);
        recieved[n] = '\0';
        printf("%d sent: %s\n", *client_fd, recieved);
        if(strcmp(recieved, "#EXIT") == 0){
            for(int i=0; i<clients.size(); i++){
                if(clients[i] == *client_fd){
                    printf("%d quit\n", *client_fd);
                    clients.erase(clients.begin() + i);
                    i--;
                    break;
                }
            }
            pthread_exit(NULL);
        }
        else{
            for(int i=0; i<clients.size(); i++){
                if(i == *client_fd) continue;
                write(clients[i], (char*)recieved, strlen(recieved));
            }
        }
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

    while(1){
        pthread_t readThread;
        int client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrsize);
        clients.push_back(client_fd);
        printf("%d people in chat-room (", clients.size());
        for(int i=0; i<clients.size(); i++){
            printf("%d, ", clients[i]);
        }
        printf(")\n");
        pthread_create(&readThread, NULL, readFunc, (void*)&clients[clients.size()-1]);
    }
    
    return 0;
}