#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <ctime>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

#define PORT 4216
#define MAXCHAR 1024

int clients[100];
int clientsize = 0;
string names[100];

void *readFunc(void *args){
    int *client_fd = (int*)args;
    string client_name;
    for(int i=0; i<clientsize; i++){
        if(clients[i] == *client_fd){
            client_name = names[i];
        }
    }

    while(1){
        char recieved[MAXCHAR];
        int n = read(*client_fd, recieved, MAXCHAR);
        recieved[n] = '\0';
        printf("%d sent: %s\n", *client_fd, recieved);
        if(strcmp(recieved, "#EXIT") == 0){
            int i;
            for(i=0; i<clientsize; i++){
                if(clients[i] == *client_fd){
                    printf("%d quit\n", *client_fd);
                    break;
                }
            }
            clientsize--;
            for(; i<clientsize; i++){
                clients[i] = clients[i+1];
                names[i] = names[i+1];
            }
            pthread_exit(NULL);
        }
        else{
            string tosend = client_name + string(": ") + string(recieved);
            const char* tosendchar = tosend.c_str();
            for(int i=0; i<clientsize; i++){
                if(*client_fd == clients[i]) continue;
                write(clients[i], (char*)tosendchar, strlen(tosendchar));
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

    while(clientsize < 100){
        pthread_t readThread;
        clients[clientsize] = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrsize);
        char name[30];
        int n = read(clients[clientsize], name, 30);
        name[n] = '\0';
        names[clientsize] = string(name);
        clientsize++;
        pthread_create(&readThread, NULL, readFunc, (void*)&clients[clientsize-1]);
    }
    
    return 0;
}