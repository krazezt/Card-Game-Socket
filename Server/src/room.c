#include "room.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

Room roomList[MAX_ROOM];

void createRoomList() {
    for (int i = 0; i < MAX_ROOM; i++) {
        roomList[i].host = 0;
        roomList[i].id = i;
        roomList[i].pointPool = 0;
        roomList[i].isPlaying = 0;
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
                    if (roomList[i].host == 0) roomList[i].host = playerPtr->sockfd;
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
                        roomList[i].players[j]->roomID = -1;
                        roomList[i].players[j]->state = 2;
                        roomList[i].players[j] = NULL;                      //Reset the slot
                        return 1;
                    }
            }
            return 0;                                                       //Khong tim thay player
        };
    }

    return 0;                                                               //Khong tim thay roomId tuong ung
}

void broadCastRoom(int screenID, int roomID, char* mes) {
    char* res = (char*)calloc(1025, sizeof(char));

    for (int i = 0; i < MAX_ROOM; i++) {
        if (roomList[i].id == roomID) {
            for (int j = 0; j < MAX_PLAYER_IN_ROOM; j++) {
                if (roomList[i].players[j] != NULL) {
                    sprintf(res, "%02d|%d|%s", screenID, j, mes);
                    send(roomList[i].players[j]->sockfd, res, strlen(res), 0);
                }
            }
        }
    }
}

void sendChatAndNotify(int roomID, char* mes) {
    char* res = (char*)calloc(1025, sizeof(char));
    sprintf(res, "%s", mes);

    for (int i = 0; i < MAX_ROOM; i++) {
        if (roomList[i].id == roomID) {
            for (int j = 0; j < MAX_PLAYER_IN_ROOM; j++) {
                if (roomList[i].players[j] != NULL) {
                    send(roomList[i].players[j]->sockfd, res, strlen(res), 0);
                }
            }
        }
    }
}

char* getResRoom(int roomID) {
    char* resRoomResult = (char*)calloc(1025, sizeof(char));

    for (int i = 0; i < MAX_ROOM; i++) {
        if (roomList[i].id == roomID) {
            for (int j = 0; j < MAX_PLAYER_IN_ROOM; j++) {
                if (roomList[i].players[j]->sockfd == roomList[i].host) {
                    sprintf(resRoomResult, "%d|", j);
                    break;
                }
            }
            break;
        };
    }

    for (int i = 0; i < MAX_ROOM; i++) {
        if (roomList[i].id == roomID) {
            for (int j = 0; j < MAX_PLAYER_IN_ROOM; j++) {
                if (roomList[i].players[j] != NULL) {
                    sprintf(resRoomResult, "%s%s", resRoomResult, getResPlayer(roomList[i].players[j]->sockfd));
                }
            }
        }
    }

    char *tmpResult = malloc(strlen(resRoomResult) + 1);
    strcpy(tmpResult, resRoomResult);
    return tmpResult;
}

char* getResRoomList() {
    char* resRoomList = (char*)calloc(1025, sizeof(char));
    char tmpHost[20];
    int count;

    sprintf(resRoomList, "02|");

    for (int i = 0; i < MAX_ROOM; i++) {
        if (roomList[i].host == 0) {
            sprintf(resRoomList, "%s%03d|---|0|0|", resRoomList, i);
        } else {
            count = 0;
            for (int j = 0; j < MAX_PLAYER_IN_ROOM; j++) {
                if (roomList[i].players[j] != NULL) {
                    count++;
                }
            }

            strcpy(tmpHost, findPlayer(roomList[i].host).name);
            sprintf(resRoomList, "%s%03d|%s|%d|%d|", resRoomList, i, tmpHost, count, roomList[i].isPlaying);
        }
        //sprintf(resRoomList, "%s\n", resRoomList);
    }

    return resRoomList;
}

int setPlayerReady(int roomID, int player_sockfd) {
    for (int i = 0; i < MAX_ROOM; i++) {
        if (roomList[i].id == roomID) {
            for (int j = 0; j < MAX_PLAYER_IN_ROOM; j++) {
                if (roomList[i].players[j]->sockfd == player_sockfd) {
                    roomList[i].players[j]->state = 4;
                    return 1;
                    break;
                }
            }
            break;
        };
    }
    return 0;
}

int setPlayerHolding(int roomID, int player_sockfd) {
    for (int i = 0; i < MAX_ROOM; i++) {
        if (roomList[i].id == roomID) {
            for (int j = 0; j < MAX_PLAYER_IN_ROOM; j++) {
                if (roomList[i].players[j]->sockfd == player_sockfd) {
                    roomList[i].players[j]->state = 3;
                    return 1;
                    break;
                }
            }
            break;
        };
    }
    return 0;
}

int kickPlayer(int curr_player_sd, int roomID, char *playerToKick) {
    char* res = (char*)calloc(1025, sizeof(char));
    for (int i = 0; i < MAX_ROOM; i++) {
        if (roomList[i].id == roomID) {
            if (curr_player_sd != roomList[i].host) return 0;     // Khong phai chu phong
            for (int j = 0; j < MAX_PLAYER_IN_ROOM; j++) {
                if (roomList[i].players[j] == NULL) return 0;
                if (strcmp(playerToKick, roomList[i].players[j]->name) == 0) {
                    strcpy(res, getResRoomList());
                    send(roomList[i].players[j]->sockfd, res, strlen(res), 0);
                    removePlayer(roomID, roomList[i].players[j]->sockfd);
                    return 1;
                    break;
                }
            }
            return 0;
            break;
        };
    }
    return 0;
}

int promotePlayer(int curr_player_sd, int roomID, char *playerToKick) {
    char* res = (char*)calloc(1025, sizeof(char));
    for (int i = 0; i < MAX_ROOM; i++) {
        if (roomList[i].id == roomID) {
            if (curr_player_sd != roomList[i].host) return 0;     // Khong phai chu phong
            for (int j = 0; j < MAX_PLAYER_IN_ROOM; j++) {
                if (roomList[i].players[j] == NULL) return 0;
                if (strcmp(playerToKick, roomList[i].players[j]->name) == 0) {
                    roomList[i].host = roomList[i].players[j]->sockfd;
                    sprintf(res, "00|#|You are the host now!|");
                    send(roomList[i].players[j]->sockfd, res, strlen(res), 0);
                    return 1;
                    break;
                }
            }
            return 0;
            break;
        };
    }
    return 0;
}