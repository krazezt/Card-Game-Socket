#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "card.h"

char* card_names[] = {
	"2 \xE2\x99\xA0",
	"2 \xE2\x99\xA3",
	"2 \xE2\x99\xA6",
	"2 \xE2\x99\xA5",
	"3 \xE2\x99\xA0",
	"3 \xE2\x99\xA3",
	"3 \xE2\x99\xA6",
	"3 \xE2\x99\xA5",
	"4 \xE2\x99\xA0",
	"4 \xE2\x99\xA3",
	"4 \xE2\x99\xA6",
	"4 \xE2\x99\xA5",
	"5 \xE2\x99\xA0",
	"5 \xE2\x99\xA3",
	"5 \xE2\x99\xA6",
	"5 \xE2\x99\xA5",
	"6 \xE2\x99\xA0",
	"6 \xE2\x99\xA3",
	"6 \xE2\x99\xA6",
	"6 \xE2\x99\xA5",
	"7 \xE2\x99\xA0",
	"7 \xE2\x99\xA3",
	"7 \xE2\x99\xA6",
	"7 \xE2\x99\xA5",
	"8 \xE2\x99\xA0",
	"8 \xE2\x99\xA3",
	"8 \xE2\x99\xA6",
	"8 \xE2\x99\xA5",
	"9 \xE2\x99\xA0",
	"9 \xE2\x99\xA3",
	"9 \xE2\x99\xA6",
	"9 \xE2\x99\xA5",
	"10 \xE2\x99\xA0",
	"10 \xE2\x99\xA3",
	"10 \xE2\x99\xA6",
	"10 \xE2\x99\xA5",
	"Jack \xE2\x99\xA0",
	"Jack \xE2\x99\xA3",
	"Jack \xE2\x99\xA6",
	"Jack \xE2\x99\xA5",
	"Queen \xE2\x99\xA0",
	"Queen \xE2\x99\xA3",
	"Queen \xE2\x99\xA6",
	"Queen \xE2\x99\xA5",
	"King \xE2\x99\xA0",
	"King \xE2\x99\xA3",
	"King \xE2\x99\xA6",
	"King \xE2\x99\xA5",
	"Ace \xE2\x99\xA0",
	"Ace \xE2\x99\xA3",
	"Ace \xE2\x99\xA6",
	"Ace \xE2\x99\xA5"
};

void printAll() {
    for (int i = 0; i < 52; i++) {
        printf("No.%d: %s\n", i, card_names[i]);
    }
}

char* getCardName(int card) {
	return card_names[card];
}

int getCardPoint(int card) {
    if (card > 47) return 1;
    else if (card > 35) return 10;
	else return (card / 4) + 2;
}

int calculatePoint(int* cards) {            // 3 cay bai`
    int count_jqk = 0, max = 0;             // neu co 3 cay JQK thi tra ve max, sau nay dung no de tinh diem neu nhieu nguoi co 3 JQK
    int result = 0;

    for (int i = 0; i < 3; i++) {
        if (cards[i] > 35 && cards[i] <= 47) {      // Dem so cay JQK
            count_jqk++;
            if (cards[i] > max) max = cards[i];
        }

        result += getCardPoint(cards[i]);
    }

    if (count_jqk == 3) return max;     // Bai co 3 cay JQK
    else return (result % 10);          // Phan le cua tong diem
}

int comparePoint(int* cards1, int* cards2) {
    int point1 = calculatePoint(cards1);
    int point2 = calculatePoint(cards2);

    if (point1 > point2) return 1;
    else if (point1 < point2) return -1;
    else {
        int max1 = 0, max2 = 0;
        for (int i = 0; i < 3; i++) {
            if (cards1[i] > max1) max1 = cards1[i];
            if (cards2[i] > max2) max2 = cards2[i]; 
        }

        if (max1 > max2) return 1;
        else return -1;
    }
}

void initRandomSeed() {
    srand(time(0));
}

int* distributeCard(int numberOfPlayers) {
    int count = numberOfPlayers*3, note[52] = {0};
    int* result = (int*)calloc(count, sizeof(int));

    int i = 0, tmp;
    while (i < count) {
        tmp = (rand() % 52);
        if (note[tmp] == 0) {
            note[tmp] = 1;
            result[i] = tmp;
            i++;
        }
    }

    return result;
}