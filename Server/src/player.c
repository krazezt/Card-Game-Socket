#include "player.h"
#include "util.h"
#include <string.h>
#include <stdlib.h>

Player playerList[MAX_PLAYER];

void newPlayerConnect(char name[10], int sockfd){
    
}

Player findPlayer(int player_sockfd) {
    for (int i = 0; i < MAX_PLAYER; i++) {
        if (playerList[i].sockfd == player_sockfd) {
            return playerList[i];
        }
    }
}

char* getResPlayer(int sockfd) {
    Player player = findPlayer(sockfd);
    char* resPlayerResult = (char*)calloc(1025, sizeof(char));

    strcpy(resPlayerResult, addElement(resPlayerResult, player.name));
    strcpy(resPlayerResult, addElement(resPlayerResult, player.state == 3 ? "0" : "1"));

    char *tmpResult = malloc(strlen(resPlayerResult) + 1);
    strcpy(tmpResult, resPlayerResult);
    return tmpResult;
}