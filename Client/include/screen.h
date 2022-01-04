#include "convertCode.h"
typedef struct RoomListInfo{
    int id;
    char name[15];
    int slot;
    int isPlaying;
} RoomList;

typedef struct RoomInfo{
    char name[15];
    int status;
} RoomInfo;

RoomList* getRoomList(char*);
RoomInfo* getPlayerList(char*);
void mainScreen();
void roomListScreen(RoomList*);
void roomScreen(RoomInfo*,int, int,int);
void messeges();
void gameScreen();
