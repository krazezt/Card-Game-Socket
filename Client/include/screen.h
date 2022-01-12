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

typedef struct GameInfo{
    char name[15];
    int point;
    int bet;
    int isLose;
} GameInfo;

typedef struct Message{
    char name[15];
    char content[1000];
} Message;

RoomList* getRoomList(char*);
RoomInfo* getPlayerList(char*);
GameInfo* getGameInfo(char*);
void getMessageList(Message*, char*,int);
int getPoint(GameInfo* ,int);
void mainScreen();
void roomListScreen(RoomList*);
void roomScreen(RoomInfo*,int, int,int);
void messeges(Message* ,int );
void gameScreen(GameInfo* ,int , int , int);
void endGame(GameInfo* ,int , int , int);
