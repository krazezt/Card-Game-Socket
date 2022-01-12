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
#define MAX 1025
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_t tmp_thread;
int ScreenNumber = 1;
int isHost = 0;
int status = 0;
int mesNumber = 0;
int  currPoint;
Message mesList[200]; 

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
    char currScreen[MAX];
    int buff_bytes;
    int *conv = (int*) var;
    int client_sock = *conv;
    int commandCode, part;
    while(1){
        buff_bytes = recv(client_sock, buff, MAX - 1, 0);
        buff[buff_bytes]='\0';
        if (buff_bytes <= 0)
        {
            printf("\nError!Cannot receive data from sever!\n");
            break;
        }//printf("%s\n",buff);
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
            	
            	pthread_mutex_lock(&mutex1);
                ScreenNumber = 2;
            	roomListScreen(getRoomList(buff));
		
		
            	pthread_cond_signal(&cond1);
            	pthread_mutex_unlock(&mutex1);
            	break;
            case 03:

                pthread_mutex_lock(&mutex1);
                ScreenNumber = 3;
                strcpy(currScreen,buff);
                currScreen[buff_bytes]='\0';
                
                
                if(atoi(readPart(buff, 2))==atoi(readPart(buff, 1))) isHost =1; else isHost = 0;
                
                //printf screen
                part = readPartLeng(buff); part = (part-3)/2;
                
            	roomScreen(getPlayerList(buff),atoi(readPart(buff, 2)),atoi(readPart(buff, 1)),part);
            	
		
            	pthread_cond_signal(&cond1);
            	pthread_mutex_unlock(&mutex1);
            	
            	
            	break;
            case 04:
                pthread_mutex_lock(&mutex1);
                ScreenNumber = 4;
                strcpy(currScreen,buff);
                currScreen[buff_bytes]='\0';
                
                currPoint = getPoint(getGameInfo(buff),atoi(readPart(buff, 1)));
                
                part = readPartLeng(buff); part = (part-3)/4;
            	gameScreen(getGameInfo(buff),atoi(readPart(buff, 2)),atoi(readPart(buff, 1)),part);
		
		
            	pthread_cond_signal(&cond1);
            	pthread_mutex_unlock(&mutex1);
            	
            	break;
            case 05:
            	pthread_mutex_lock(&mutex1);
                ScreenNumber = 5;
                strcpy(currScreen,buff);
                currScreen[buff_bytes]='\0';
                
                currPoint = getPoint(getGameInfo(buff),atoi(readPart(buff, 1)));
                
                part = readPartLeng(buff); part = (part-3)/4;
            	endGame(getGameInfo(buff),atoi(readPart(buff, 2)),atoi(readPart(buff, 1)),part);
		
		
            	pthread_cond_signal(&cond1);
            	pthread_mutex_unlock(&mutex1);
            	break;
            case 00:
                pthread_mutex_lock(&mutex1);
                Message newMes;
            	strcpy(newMes.name,readPart(buff, 1));
		strcpy(newMes.content,readPart(buff, 2));
		mesList[mesNumber] = newMes;
		/*
		if(ScreenNumber == 3){
                   part = readPartLeng(currScreen); part = (part-3)/2;
                
            	    roomScreen(getPlayerList(currScreen),atoi(readPart(currScreen, 2)),atoi(readPart(currScreen, 1)),part);
		}
		
		
		if(ScreenNumber == 4){
		    part = readPartLeng(currScreen); part = (part-3)/4;
            	    gameScreen(getGameInfo(currScreen),atoi(readPart(currScreen, 2)),atoi(readPart(currScreen, 1)),part);
		}
		*/
		
		
            	mesNumber++;
            	messeges(mesList,mesNumber);
            	
            	pthread_cond_signal(&cond1);
            	pthread_mutex_unlock(&mutex1);
            	
            	break;
            default:
            	
            	break;
        }
        
    }
}


void * sender(void* var){
    char buff[MAX];
    char tmp[MAX];
    int buff_bytes;
    int *conv = (int*) var;
    int client_sock = *conv;

    int roomId, point, check, choice;
    char* mes;
    char* mes_type;
    char nickname[15];
	tmp_thread = pthread_self();
    while(1){
    	pthread_mutex_lock(&mutex1);
        pthread_cond_wait(&cond1, &mutex1);
        pthread_mutex_unlock(&mutex1);
        fflush(stdin);
        scanf("%d",&choice); 

		switch (choice)
		{
		case 1:
			//go to roomlist
			if(ScreenNumber == 1){
				mes_type = "000|";

            	send(client_sock, mes_type, strlen(mes_type), 0);
			}
			
			//get in room
			if(ScreenNumber == 2){
				mes = NULL;
                	mes = calloc(MAX, sizeof(char));
                	fflush(stdin);
                	do{
                	printf("enter room id: "); scanf("%d",&roomId); 
                	if(roomId<0 || roomId>9) printf("Unknown room ID!\n");
                	}while(roomId<0 || roomId>9);
                	sprintf (mes, "002|%d", roomId);
                	printf("enter your nickname: "); scanf("%s",nickname); 
                	
                	sprintf (mes, "%s|%s|", mes,nickname);

                	send(client_sock, mes, strlen(mes), 0);
			}

			//switch status
			if(ScreenNumber == 3){
				if(status == 0){
                    	    mes_type = "005|";
                    	    status = 1;
                    	    
                    	    send(client_sock, mes_type, strlen(mes_type), 0);
                    	}else{
                    	    mes_type = "006|";
                    	    status = 0;
                    	    
                    	    send(client_sock, mes_type, strlen(mes_type), 0);
                    	}
			}

			//chat
			if(ScreenNumber == 4 || ScreenNumber == 5){
				mes = NULL;
                	mes = calloc(MAX, sizeof(char));
                    	printf("Enter your message: ");
                    	if(fgets(buff,MAX,stdin) != NULL);
                    	fgets(buff,MAX,stdin);
                    	buff[strlen(buff)-1] = '\0';
                    	sprintf (mes, "004|%s|", buff);
                    	choice = 0;
                    	send(client_sock, mes, strlen(mes), 0);
                    	free(mes);
			}

			
			break;
		
		case 2:
			//exit
			if(ScreenNumber == 1){
				exit(1);
			}

			//return
			if(ScreenNumber == 2 || ScreenNumber == 5){
				mes_type = "003|";
                	ScreenNumber--;
					if(ScreenNumber == 5){ScreenNumber--;}
                	send(client_sock, mes_type, strlen(mes_type), 0);
                	
			}
			
			//chat
			if(ScreenNumber == 3){
				mes = NULL;
                    	mes = calloc(MAX, sizeof(char));
                    	printf("Enter your message: ");
                    	fgets(buff,MAX,stdin);
                    	
                    	
                    	if(buff[0] == '\n'){
                           fgets(buff,MAX,stdin);
                    	    
                    	}
                    	sprintf (mes, "004|%s|", buff);
                    	mes[strlen(mes)-2] = '|';
                    	mes[strlen(mes)-1] = '\0';
                    	choice = 0;
                    	send(client_sock, mes, strlen(mes), 0);
                    	free(mes);
			}
			
			//bet
			if(ScreenNumber == 4){
				
                	mes = NULL;
                	mes = calloc(MAX, sizeof(char));
                	printf("Enter point to bet: "); scanf("%d",&point); 
                	
                    	sprintf (mes, "011|%d|", point);
                    	choice = 0;
                    	send(client_sock, mes, strlen(mes), 0);
                    	free(mes);
			}
			
			break;
		
		case 3:
			//kick player
			if(ScreenNumber == 3){
				mes = NULL;
                 	mes = calloc(MAX, sizeof(char));
                 	if(isHost == 1){
                    	printf("Who will be kicked: ");
                    	scanf("%s",nickname); 
                    	sprintf (mes, "007|%s|", nickname);
                    	
                    	send(client_sock, mes, strlen(mes), 0);
                    	}else{
                    	    
                	    mes_type = "003|";
                	    ScreenNumber--;
                	    send(client_sock, mes_type, strlen(mes_type), 0);
                	    
						}
					free(mes);
			}
			
			//get card
			if(ScreenNumber == 4){
				if(isHost == 1){
                 	
                	mes_type = "010|";
                	send(client_sock, mes_type, strlen(mes_type), 0);
				}
			}
			break;

		case 4:
			//allow to host
			if(ScreenNumber == 3){
				if(isHost == 1){
                 	check = 0;
                 	mes = NULL;
                 	mes = calloc(MAX, sizeof(char));
                    	printf("Who will be allowed: ");
                    	scanf("%s",nickname); 
                    	sprintf (mes, "008|%s|", nickname);
                    	choice = 0;
                    	send(client_sock, mes, strlen(mes), 0);
                    	}else{
                    	 
                    	 printf("enter your choice: ");
						}
					free(mes);
			}
			break;

		case 5:
			//start game
			if(ScreenNumber == 3){
				if(isHost == 1){
                    mes_type = "009|";
            	    choice = 0;
            	    send(client_sock, mes_type, strlen(mes_type), 0);
				}
			}
			break;
		
		case 6:
			//quit
			if(ScreenNumber == 3){
				if(isHost == 1){
                 	check = 0;
                	mes_type = "003|";
                	ScreenNumber--;
                	send(client_sock, mes_type, strlen(mes_type), 0);
                	}else{
                    	 
                    	 printf("enter your choice: ");
					}
			}
			break;
		default:
			printf("Wrong number!");
			break;
		}

/*        switch(ScreenNumber){
            case 1:
            	check = 1;
            	while(check == 1) {
                mes_type = "000|";
    	        
                scanf("%d",&choice); 
                if(choice == 1){
                	check = 0;
                	choice = 0;
                	send(client_sock, mes_type, strlen(mes_type), 0);
                	
                		
                }else if(choice == 2){
                	exit(1);
                }else {printf("Wrong number\n");
                if(fgets(tmp,MAX,stdin)!=NULL);
                printf("enter your choice: ");}
		}
                
                break;
            case 2:
		check =1;
		while(check == 1){
     
                scanf("%d",&choice); 
                	
                if(choice == 1){
                	check = 0;
                	mes = NULL;
                	mes = calloc(MAX, sizeof(char));
                	fflush(stdin);
                	do{
                	printf("enter room id: "); scanf("%d",&roomId); 
                	if(roomId<0 || roomId>9) printf("Unknown room ID!\n");
                	}while(roomId<0 || roomId>9);
                	sprintf (mes, "002|%d", roomId);
                	printf("enter your nickname: "); scanf("%s",nickname); 
                	
                	sprintf (mes, "%s|%s|", mes,nickname);
                	choice = 0;

                	send(client_sock, mes, strlen(mes), 0);
                }else if(choice == 2){
                	check = 0;
                	mes_type = "003|";
                	ScreenNumber--;
                	send(client_sock, mes_type, strlen(mes_type), 0);
                	free(mes);
                }else {if(fgets(tmp,MAX,stdin)!=NULL);
                	printf("enter your choice: ");
                }
		}
                
            	break;
            case 3:
            	check =1;
            	choice =0;
		while(check == 1){
		
     
                scanf("%d",&choice);

                if(choice ==1){

                    	check = 0;
                    	if(status == 0){
                    	    mes_type = "005|";
                    	    status = 1;
                    	    choice = 0;
                    	    send(client_sock, mes_type, strlen(mes_type), 0);
                    	}else{
                    	    mes_type = "006|";
                    	    status = 0;
                    	    choice = 0;
                    	    send(client_sock, mes_type, strlen(mes_type), 0);
                    	}
                    	
                }
                        
                 else if(choice == 2){
                    	check = 0;
                    	mes = NULL;
                    	mes = calloc(MAX, sizeof(char));
                    	printf("Enter your message: ");
                    	fgets(buff,MAX,stdin);
                    	
                    	
                    	if(buff[0] == '\n'){
                           fgets(buff,MAX,stdin);
                    	    
                    	}
                    	sprintf (mes, "004|%s|", buff);
                    	mes[strlen(mes)-2] = '|';
                    	mes[strlen(mes)-1] = '\0';
                    	choice = 0;
                    	send(client_sock, mes, strlen(mes), 0);
                    	free(mes);
                 }
                 else if(choice ==3){
                 	check = 0;
                 	mes = NULL;
                 	mes = calloc(MAX, sizeof(char));
                 	if(isHost == 1){
                    	printf("Who will be kicked: ");
                    	scanf("%s",nickname); 
                    	sprintf (mes, "007|%s|", nickname);
                    	choice = 0;
                    	send(client_sock, mes, strlen(mes), 0);
                    	}else{
                    	    check = 0;
                	    mes_type = "003|";
                	    ScreenNumber--;
                	    send(client_sock, mes_type, strlen(mes_type), 0);
                	    
			}
			free(mes);
                 }
                 else if(choice ==4){
                 	if(isHost == 1){
                 	check = 0;
                 	mes = NULL;
                 	mes = calloc(MAX, sizeof(char));
                    	printf("Who will be allowed: ");
                    	scanf("%s",nickname); 
                    	sprintf (mes, "008|%s|", nickname);
                    	choice = 0;
                    	send(client_sock, mes, strlen(mes), 0);
                    	}else{
                    	 
                    	 printf("enter your choice: ");
			}
			free(mes);
                 }
                 else if(choice ==5){
                 	check = 0;
                    	if(isHost == 1){
                    	    mes_type = "009|";
                    	    choice = 0;
                    	    send(client_sock, mes_type, strlen(mes_type), 0);
                    	}else{
                    	 
                    	 printf("enter your choice: ");
			}
			
                 }
                 else if(choice ==6){
                 	if(isHost == 1){
                 	check = 0;
                	mes_type = "003|";
                	ScreenNumber--;
                	send(client_sock, mes_type, strlen(mes_type), 0);
                	}else{
                    	 
                    	 printf("enter your choice: ");
			}
                 }
                 else{
                    	 if(fgets(tmp,MAX,stdin)!=NULL);
                    	 printf("enter your choice: ");
			}
                	
                }
            	
                break;
            case 4:
            	check =1;
		while(check == 1){
     
                scanf("%d",&choice); 
                	
                if(choice == 1){
                	check = 0;
                	mes = NULL;
                	mes = calloc(MAX, sizeof(char));
                    	printf("Enter your message: ");
                    	if(fgets(buff,MAX,stdin) != NULL);
                    	fgets(buff,MAX,stdin);
                    	buff[strlen(buff)-1] = '\0';
                    	sprintf (mes, "004|%s|", buff);
                    	choice = 0;
                    	send(client_sock, mes, strlen(mes), 0);
                    	free(mes);
                }else if(choice == 2){
                	check = 0;
                	mes = NULL;
                	mes = calloc(MAX, sizeof(char));
                	printf("Enter point to bet: "); scanf("%d",&point); 
                	
                    	sprintf (mes, "011|%d|", point);
                    	choice = 0;
                    	send(client_sock, mes, strlen(mes), 0);
                    	free(mes);
                }else if(choice == 3){
                	if(isHost == 1){
                 	check = 0;
                	mes_type = "010|";choice = 0;
                	send(client_sock, mes_type, strlen(mes_type), 0);
                	}else{
                    	 
                    	 printf("enter your choice: ");
			}
                }else{if(fgets(tmp,MAX,stdin)!=NULL);
                	printf("enter your choice: ");
                }
		}
                
            	break;
            case 5:
            	check =1;
		while(check == 1){
     
                scanf("%d",&choice); 
                	
                if(choice == 1){
                	check = 0;
                	mes = NULL;
                	mes = calloc(MAX, sizeof(char));
                    	printf("Enter your message: ");
                    	if(fgets(buff,MAX,stdin) != NULL);
                    	fgets(buff,MAX,stdin);
                    	buff[strlen(buff)-1] = '\0';
                    	sprintf (mes, "004|%s|", buff);
                    	choice = 0;
                    	send(client_sock, mes, strlen(mes), 0);
                    	free(mes);
                }else if(choice == 2){
                	check = 0;
                	mes_type = "003|";
                	ScreenNumber--;
                	send(client_sock, mes_type, strlen(mes_type), 0);
                
                }else{if(fgets(tmp,MAX,stdin)!=NULL);
                	printf("enter your choice: ");
                }
		}
            	break;
            default:
            	if(fgets(tmp,MAX,stdin)!=NULL); 
            	break;
        }
    */
	}
}


int main(int argc, char *argv[]){
    struct sockaddr_in address;
    pthread_t tid1, tid2;
    int client_sock;
    int oldScreen = ScreenNumber;
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



    //Step 4: Close socket
    close(client_sock);
    return 0;
    
}
