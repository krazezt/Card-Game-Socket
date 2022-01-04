#define MAX_PLAYER 30

typedef struct {
    int roomID;
    char name[11];
    int cards[3];
    int state;
    int sockfd;
    int bet;
    int point;
    int lose;
} Player;

void initPlayerList();
Player findPlayer(int);
char* getResPlayer(int);

// state:
// 1: Dang o man hinh main menu.
// 2: Dang o man hinh room list.
// 3. Dang o man hinh room, holding.
// 4. Dang o man hinh room, readying.
// 5. Dang o man hinh game, chua nhap diem cuoc.
// 6. Dang o man hinh game, da nhap diem cuoc.