#include "player.h"

#define MAX_PLAYER_IN_ROOM 6
#define MAX_ROOM 10

typedef struct {
    int id;
    Player* players[MAX_PLAYER_IN_ROOM];
    int pointPool;
    int host;
} Room;

void createRoomList();
int addPlayer(int, Player*);
int removePlayer(int, int);