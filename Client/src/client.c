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

#include "screen.h"
#include "convertCode.h"

#define TRUE 1
#define FALSE 0
#define PORT 8888
#define MAX 1024
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;

int ScreenNumber = 1;

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
    int commandCode;
    while(1){
        buff_bytes = recv(client_sock, buff, MAX - 1, 0);
        buff[buff_bytes]='\0';
        if (buff_bytes <= 0)
        {
            printf("\nError!Cannot receive data from sever!\n");
            break;
        }
        commandCode = readCommandCode(buff);
        switch(commandCode){
            case 01:
            	pthread_mutex_lock(&mutex1);
            	ScreenNumber = 1;
            	
            	mainScreen();
            	
            	pthread_cond_signal(&cond1);
            	pthread_mutex_unlock(&mutex1);
            	break;
            case 02:
            	ScreenNumber = 2;
            	pthread_mutex_lock(&mutex1);
                
            	roomListScreen(getRoomList(buff));
            	
            	pthread_cond_signal(&cond1);
            	pthread_mutex_unlock(&mutex1);
            	break;
            case 03:
                ScreenNumber = 3;
            	printf("%s\n",buff);
            	
            	break;
            case 04:
                ScreenNumber = 4;
            	
            	break;
            case 00:
                ScreenNumber = 4;
            	
            	break;
            default:
            	
            	break;
        }
        
    }
}


void * sender(void* var){
    char buff[MAX];
    int buff_bytes;
    int *conv = (int*) var;
    int client_sock = *conv;
    int choice;
    char roomId[3];
    char* mes;
    char nickname[15];

    while(1){
    	pthread_mutex_lock(&mutex1);
        pthread_cond_wait(&cond1, &mutex1);
        switch(ScreenNumber){
            case 1:
                mes = "000";
    	        
                do {
                	printf("enter your choice: ");
     
                	scanf("%d",&choice); 
                	if(choice == 1){
                		send(client_sock, mes, strlen(mes), 0);
                		
                	}else if(choice == 2){
                		exit(1);
                	}else {printf("Wrong");}
                }while(choice != 1 && choice != 2);
                pthread_mutex_unlock(&mutex1);
                break;
            case 2:

                do {
                	printf("enter your choice: ");
     
                	scanf("%d",&choice); 
                	
                	if(choice == 1){
                		mes = NULL;
                		mes = calloc(25, sizeof(char));
                		fflush(stdin);
                		printf("enter room id: "); scanf("%s",roomId); 
                		sprintf (mes, "002|%s", roomId);
                		printf("enter your nickname: "); scanf("%s",nickname); 
                		
                		sprintf (mes, "%s|%s|", mes,nickname);

                		printf("%s\n",mes);
                		send(client_sock, mes, strlen(mes), 0);
                	}else if(choice == 2){
                		exit(1);
                	}else {printf("Wrong number!\n");}
                }while(choice != 1 && choice != 2);
                pthread_mutex_unlock(&mutex1);
            	break;
            case 3:
            	break;
            case 4:
            	break;
            default:
            	
            	break;
        }
    }
}


int main(int argc, char *argv[]){
    struct sockaddr_in address;
    pthread_t tid1, tid2;
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
    pthread_create(&tid1,NULL,&reader,&client_sock);
    pthread_create(&tid2,NULL,&sender,&client_sock);
    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
    

    //Step 4: Close socket
    close(client_sock);
    return 0;
    
}
