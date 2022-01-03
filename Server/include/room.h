#include "player.h"

#define MAX_PLAYER_IN_ROOM 6
#define MAX_ROOM 10

typedef struct {
    int id;
    Player* players[MAX_PLAYER_IN_ROOM];
    int pointPool;
    int host;
    int isPlaying;
} Room;

void createRoomList();
int addPlayer(int roomID, Player* playerPtr);
int removePlayer(int roomID, int player_sockfd);
void broadCastRoom(int screenID, int roomID, char* mes);
char* getResRoom(int roomID);
char* getResRoomList();
void sendChatAndNotify(int roomID, char* mes);
int setPlayerReady(int roomID, int player_sockfd);
int setPlayerHolding(int roomID, int player_sockfd);
int kickPlayer(int curr_player_sd, int roomID, char *playerToKick);
int promotePlayer(int curr_player_sd, int roomID, char *playerToKick);
int startGame(int curr_player_sd, int roomID);
char* getResGame(int roomID);
