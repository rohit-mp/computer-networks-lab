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

#define PORT 4210
#define MAXCHAR 1024

vector<int> clients;

struct arg_struct{
    struct sockaddr_in arg1;
    int arg2;
    int arg3;
    int ret;
};

void *acceptFunc(void *args){
    struct arg_struct *arg = (struct arg_struct*) args;
    struct sockaddr_in address = arg->arg1;
    int server_fd = arg->arg2;
    int addrsize = arg->arg3;
    int client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrsize);
    printf("%d, ", client_fd);
    clients.push_back(client_fd);
    arg->ret = client_fd;
}

void *readFunc(void *args){
    int *client_fd = (int*)args;

    while(1){
        char recieved[MAXCHAR];
        int n = read(*client_fd, recieved, MAXCHAR);
        recieved[n] = '\0';
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

    struct arg_struct acceptargs;
    acceptargs.arg1 = address;
    acceptargs.arg2 = server_fd;
    acceptargs.arg2 = addrsize;
    while(1){
        pthread_t acceptThread, readThread;
        pthread_create(&acceptThread, NULL, acceptFunc, (void*)&acceptargs);
        int client_fd = acceptargs.ret;
        pthread_join(acceptThread, NULL); //ensures ret value is not lost but another accept
        pthread_create(&readThread, NULL, readFunc, (void*)&client_fd);
    }
    
    return 0;
}