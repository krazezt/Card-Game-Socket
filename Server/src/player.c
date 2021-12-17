#include "player.h"
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