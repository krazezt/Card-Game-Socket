#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <pthread.h>

#define TRUE 1
#define FALSE 0
#define PORT 8888
#define MAX 1024


int isValidIpAddress(char *ipAddress)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    if (result != 0)
        return 1;
    else
        return 0;
}

void * reader(void* var){
    char buff[MAX];
    int buff_bytes;
    int *conv = (int*) var;
    int client_sock = *conv;
    while(1){
        buff_bytes = recv(client_sock, buff, MAX - 1, 0);
        if (buff_bytes <= 0)
        {
            printf("\nError!Cannot receive data from sever!\n");
            break;
        }
        buff[buff_bytes] = '\0';
        printf("%s\n", buff);
    }
}

void messenger(void *var){

}

int main(int argc, char *argv[]){
    struct sockaddr_in address;
    pthread_t tid;
    int client_sock;

    if (argc != 2)
    {
        printf("error, too many or too few arguments\n");
        printf("Correct format is /.client YourId\n");
        exit(1);
    }
    //check if input id is valid
    if (isValidIpAddress(argv[1]) == 0)
    {
        printf("%s Not a valid ip address\n", argv[1]);
        return 1;
    }
    //Step 1: Construct socket
    if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }
    //Step 2: Specify server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(argv[1]);
    address.sin_port = htons(PORT);
    
    //Step 3: Request to connect server
    if (connect(client_sock, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        printf("\nError!Can not connect to sever! Client exit immediately! ");
        return 0;
    }
    
    printf("Listener on port %d \n", PORT);
    //Step 4: Communicate with server
    
    while(TRUE){
        pthread_create(&tid,NULL,&reader,&client_sock);
    }
    //Step 4: Close socket
    close(client_sock);
    return 0;
    
}
