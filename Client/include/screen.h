#include "convertCode.h"
typedef struct RoomListInfo{
    int id;
    char name[15];
    int slot;
    int isPlaying;
} RoomList;
RoomList* getRoomList(char*);
void mainScreen();
void roomListScreen(RoomList*);
void roomScreen();
void messeges();
void gameScreen();
