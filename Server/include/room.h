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
int addPlayer(int, Player*);
int removePlayer(int, int);
void broadCastRoom(int, int, char*);
char* getResRoom(int);
char* getResRoomList();
void sendChatAndNotify(int, char*);
int setPlayerReady(int, int);
int setPlayerHolding(int, int);