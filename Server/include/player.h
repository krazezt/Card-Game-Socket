#define MAX_PLAYER 30

typedef struct {
    int roomID;
    char name[11];
    int cards[3];
    int state;
    int sockfd;
    int role;
    int point;
    int isFollow;
} Player;

Player findPlayer(int);