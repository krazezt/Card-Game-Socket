#include "screen.h"
#include "convertCode.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

RoomList* getRoomList(char* buff){
    RoomList* rooms = (RoomList*)malloc(10*sizeof(RoomList));
    int x = 1;
    for(int i =0; i<10;i++){
        (rooms+i)->id = atoi(readPart(buff, x)); x++;
        strcpy((rooms+i)->name,readPart(buff, x)); x++;
        (rooms+i)->slot = atoi(readPart(buff, x)); x++;
        (rooms+i)->isPlaying = atoi(readPart(buff, x)); x++;
    }
    return rooms;
}

RoomInfo* getPlayerList(char* buff){
    RoomInfo* players = (RoomInfo*)malloc(6*sizeof(RoomInfo));
    int part = readPartLeng(buff); part = (part-3)/2;
    int x = 3;
    for(int i =0; i<part;i++){
        strcpy((players+i)->name,readPart(buff, x)); x++;
        (players+i)->status = atoi(readPart(buff, x)); x++;
    }
    return players;
}

GameInfo* getGameInfo(char* buff){
    GameInfo* players = (GameInfo*)malloc(6*sizeof(GameInfo));
    int part = readPartLeng(buff); part = (part-3)/4;
    int x = 3;
    for(int i =0; i<part;i++){
        strcpy((players+i)->name,readPart(buff, x)); x++;
        (players+i)->point = atoi(readPart(buff, x)); x++;
        (players+i)->bet = atoi(readPart(buff, x)); x++;
        (players+i)->isLose = atoi(readPart(buff, x)); x++;
    }
    return players;
}

int getPoint(GameInfo* players, int you){
    int result = (players+you)->point;
    return result;
}


void mainScreen(){
    
    printf("\n+=====================================+\n");
    printf("|         Welcome to the game         |\n");
    printf("+=====================================+\n");
    printf("|1. Room list.                        |\n");
    printf("|2. Quit.                             |\n");
    printf("+=====================================+\n");

    
};

void roomListScreen(RoomList* rooms){
    printf("\n/////////////////////////////////////////////////////////////////////////\n");
    printf("+=========+==================+========+=========+\n");
    printf("| Room ID |       Host       |  Slot  | Playing |\n");
    //printf("|  00001  |    ThienNT123    |  5/6   |         |\n");
    for(int i =0; i<10; i++){
    	char host[15] = {' '};
    	strcpy(host, (rooms+i)->name);
    	for(int j = strlen((rooms+i)->name); j<15;j++)
    	{host[j] = ' ';}
    	host[15] = '\0';
    	char x;
    	if((rooms+i)->isPlaying == 0) x = ' '; else x = 'x';
    	printf("|   00%d   |  %s |  %d/6   |    %c    |\n",(rooms+i)->id,host,(rooms+i)->slot,x);
    }
    printf("\n+=========+==================+========+=========+\n");
    printf("1.Join room\n");
    printf("2.Quit\n");
    printf("-------------------------------------------------\n");

};

void roomScreen(RoomInfo* players,int host, int you, int part){
    printf("\n/////////////////////////////////////////////////////////////////////////\n");
    printf("+=====+==================+==========+======+\n");
    printf("|     |     Nickname     |  Status  | Host |\n");
    //printf("|     |    ThienNT123    |  Holding |      |\n");
    
    for(int i =0; i<part; i++){

    	char person[15] = {' '};
    	strcpy(person, (players+i)->name);
    	for(int j = strlen((players+i)->name); j<15;j++)
    	{person[j] = ' ';}
    	person[15] = '\0';
    	if(i == you) printf("| you "); else printf("|     ");
    	printf("| %s  ",person);
    	if((players+i)->status == 0) printf("|  Holding "); else printf("|   Ready  ");
    	if(i == host) printf("|  x   |\n"); else printf("|      |\n");
    }
    for(;part<6;part++) printf("|     |   ---            |          |      |\n");
    printf("+=====+==================+==========+======+\n");
    if(you == host){
    	printf("1.Switch status\n");
    	printf("2.Chat\n");
    	printf("3.Kick player\n");
    	printf("4.Allow to Host\n");
    	printf("5.Start game\n");
    	printf("6.Quit\n");
    }else{
    	printf("1.Switch status\n");
    	printf("2.Chat\n");
    	printf("3.Quit\n");
    }
    printf("--------------------------------------------\n");

};

void gameScreen(GameInfo* players,int host, int you, int part){
    printf("\n/////////////////////////////////////////////////////////////////////////\n");
    printf("+=====+==================+=========+=========+======+======+\n");
    printf("|     |     Nickname     |  Point  |   Bet   | Lose | Host |\n");
    //printf("|     |    ThienNT123    |  8000   | 2000  |      |      |\n");
    for(int i =0; i<part; i++){

    	char person[15] = {' '};
    	char point[8] = {' '};
    	char bet[8] = {' '};
    	strcpy(person, (players+i)->name);
    	for(int j = strlen((players+i)->name); j<15;j++)
    	{person[j] = ' ';}
    	person[15] = '\0';   
    	if(i == you) printf("| you "); else printf("|     ");
    	printf("| %s  ",person);
    	sprintf(point,"%d ",(players+i)->point); 
    	sprintf(bet,"%d ",(players+i)->bet);	  
    	printf("| %s",point);  for (int j=0; j<8-strlen(point);j++){printf(" ");}
    	if((players+i)->bet!=0){printf("| %s",bet);    for (int j=0; j<8-strlen(bet);j++){printf(" ");}
    	}else{printf("|    0    ");}
    	if((players+i)->isLose == 1) printf("|  x   "); else printf("|      ");
    	if(i == host) printf("|  x   |\n"); else printf("|      |\n");
    }
    for(;part<6;part++) printf("|     |                  |         |         |      |      |\n");
    printf("+=====+==================+=========+=========+======+======+\n");
    if(you == host){
    	printf("1.chat\n");
    	printf("2.bet\n");
    	printf("3.get card\n");

    }else{
    	printf("1.chat\n");
	printf("2.bet\n");
    }
    printf("----------------------------------------------------------\n");

};

void endGame(GameInfo* players,int host, int you, int part){
    printf("\n/////////////////////////////////////////////////////////////////////////\n");
    printf("+=====+==================+=========+=========+======+======+\n");
    printf("|     |     Nickname     |  Point  |   Bet   | Lose | Host |\n");
    //printf("|     |    ThienNT123    |  8000   | 2000  |      |      |\n");
    for(int i =0; i<part; i++){

    	char person[15] = {' '};
    	char point[8] = {' '};
    	char bet[8] = {' '};
    	strcpy(person, (players+i)->name);
    	for(int j = strlen((players+i)->name); j<15;j++)
    	{person[j] = ' ';}
    	person[15] = '\0';   
    	if(i == you) printf("| you "); else printf("|     ");
    	printf("| %s  ",person);
    	sprintf(point,"%d ",(players+i)->point); 
    	sprintf(bet,"%d ",(players+i)->bet);	  
    	printf("| %s",point);  for (int j=0; j<8-strlen(point);j++){printf(" ");}
    	if((players+i)->bet!=0){printf("| %s",bet);    for (int j=0; j<8-strlen(bet);j++){printf(" ");}
    	}else{printf("|    0    ");}
    	if((players+i)->isLose == 1) printf("|  x   "); else printf("|      ");
    	if(i == host) printf("|  x   |\n"); else printf("|      |\n");
    }
    for(;part<6;part++) printf("|     |                  |         |         |      |      |\n");
    printf("+=====+==================+=========+=======+========+======+\n");

    	printf("1.chat\n");
    	printf("2.quit\n");

    printf("----------------------------------------------------------\n");

};

void messeges(Message mesList[],int mesNumber){
    //message screen
            	
            	if(mesNumber < 10){
            	for(int i= 0; i<mesNumber;i++){
            	if(strcmp(mesList[i].name , "#") == 0)
            	printf("server:: %s\n",mesList[i].content);
            	else printf("%s: %s\n",mesList[i].name,mesList[i].content);}
    	    	
    	    	}else{
    	    	for(int i= mesNumber - 10; i<mesNumber;i++){
            	if(strcmp(mesList[i].name , "#") == 0)
            	printf("server:: %s\n",mesList[i].content);
            	else printf("%s: %s\n",mesList[mesNumber].name,mesList[mesNumber].content);}
    	    	}
    	    	
    	    	printf("----------------------------------------------------------\n");
    		//
    		
};

