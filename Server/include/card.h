#define DECK_SIZE 52

char* getCardName(int card);
int getCardPoint(int card);
int comparePoint(int* cards1, int* cards2);
void initRandomSeed();
int* distributeCard(int numberOfPlayers);
void printAll();