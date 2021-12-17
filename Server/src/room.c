#include "room.h"
#include <stdlib.h>
#include <stdio.h>

Room roomList[MAX_ROOM];

void createRoomList() {
    for (int i = 0; i < MAX_ROOM; i++) {
        roomList[i].host = 0;
        roomList[i].id = i;
        roomList[i].pointPool = 0;
    }
}

int addPlayer(int roomID, Player* playerPtr) {                  //Them player vao phong, return 0 neu that bai, 1 neu thanh cong.
    int i = 0;
    while (1) {
        if (i >= MAX_ROOM) return 0;                            //Khong tim thay roomId tuong ung
        if (roomList[i].id == roomID) {
            for (int j = 0; j < MAX_PLAYER_IN_ROOM; j++) {      //Tim kiem tat ca nguoi choi hien co trong phong.
                if (roomList[i].players[j] == NULL) {           //Tim thay slot trong
                    roomList[i].players[j] = playerPtr;         //Cho playerPtr vao slot do
                    roomList[i].players[j]->roomID == roomID;
                    roomList[i].players[j]->state = 3;
                    return 1;
                }
            }
            return 0;                                           //Khong con slot trong nao
        } else i++;
    }

    return 0;
}

int removePlayer(int roomID, int player_sockfd) {                           //Xoa player ra khoi phong, return 0 neu that bai, 1 neu thanh cong.
    for (int i = 0; i < MAX_ROOM; i++) {
        if (roomList[i].id == roomID) {
            for (int j = 0; j < MAX_PLAYER_IN_ROOM; j++) {                  //Tim kiem tat ca nguoi choi hien co trong phong.
                if (roomList[i].players[j] != NULL)
                    if (roomList[i].players[j]->sockfd == player_sockfd) {  //Tim thay player tuong ung
                        roomList[i].players[j] = NULL;                 //Reset the slot
                        return 1;
                    }
            }
            return 0;                                                       //Khong tim thay player
        };
    }

    return 0;                                                               //Khong tim thay roomId tuong ung
}