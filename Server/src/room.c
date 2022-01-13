#include "room.h"
#include "util.h"
#include "card.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
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

int countPlayer(int roomID) {
    int count = 0;

    for (int i = 0; i < MAX_ROOM; i++) {
        if (roomList[i].id == roomID) {
            for (int j = 0; j < MAX_PLAYER_IN_ROOM; j++) {
                if (roomList[i].players[j] != NULL) count++;
            }
            break;
        };
    }

    return count;
}

int countPlaying(int roomID) {
    int count = 0;

    for (int i = 0; i < MAX_ROOM; i++) {
        if (roomList[i].id == roomID) {
            for (int j = 0; j < MAX_PLAYER_IN_ROOM; j++) {
                if (roomList[i].players[j] != NULL) 
                    if (roomList[i].players[j]->lose == 0) count++;
            }
            break;
        };
    }

    return count;
}

int findPlayerInRoom(int roomID, char* name) {
    for (int i = 0; i < MAX_PLAYER_IN_ROOM; i++) {
        if (roomList[roomID].players[i] != NULL)
            if (strcmp(roomList[roomID].players[i]->name, name) == 0) return 1;
    }
    return 0;
}

int addPlayer(int roomID, Player* playerPtr) {
    if (findPlayerInRoom(roomID, playerPtr->name)) return 0;
    
    int i = 0;
    while (1) {
        if (i >= MAX_ROOM) return 0;
        if (roomList[i].id == roomID) {
            for (int j = 0; j < MAX_PLAYER_IN_ROOM; j++) {
                if (roomList[i].players[j] == NULL) {
                    roomList[i].players[j] = playerPtr;
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

void autoPickHost(int roomID) {
    for (int i = 0; i < MAX_PLAYER_IN_ROOM; i++) {
        if (roomList[roomID].players[i] != NULL) {
            roomList[roomID].host = roomList[roomID].players[i]->sockfd;
            return;
        }
    }

    roomList[roomID].host = 0;
}

int removePlayer(int roomID, int player_sockfd) {
    char* res = (char*)calloc(1025, sizeof(char));

    for (int j = 0; j < MAX_PLAYER_IN_ROOM; j++) {
           if (roomList[roomID].players[j] != NULL)
                if (roomList[roomID].players[j]->sockfd == player_sockfd) {

                roomList[roomID].players[j]->roomID = -1;
                roomList[roomID].players[j]->state = 2;
                roomList[roomID].players[j] = NULL;                      //Reset the slot
                if (roomList[roomID].host == player_sockfd) autoPickHost(roomID);
                sprintf(res, "%s", getResRoomList());
                send(player_sockfd, res, strlen(res), 0);
                return 1;
            }
    }

    return 0;                                                               //Khong tim thay roomId tuong ung
}

int removePlayer2(int roomID, int player_sockfd) {
    char* res = (char*)calloc(1025, sizeof(char));

    for (int j = 0; j < MAX_PLAYER_IN_ROOM; j++) {
        if (roomList[roomID].players[j] != NULL)
            if (roomList[roomID].players[j]->sockfd == player_sockfd) {
                if (roomList[roomID].players[j]->state > 4) roomList[roomID].pointPool -= roomList[roomID].players[j]->point;
                roomList[roomID].players[j]->roomID = -1;
                roomList[roomID].players[j]->state = 2;
                roomList[roomID].players[j] = NULL;
                if (roomList[roomID].host == player_sockfd) autoPickHost(roomID);
                return 1;
            }
    }

    return 0;
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
    int hostIndex = 0;

    for (int i = 0; i < MAX_ROOM; i++) {
        if (roomList[i].id == roomID) {
            for (int j = 0; j < MAX_PLAYER_IN_ROOM; j++) {
                if (roomList[i].players[j] != NULL) {
                    if (roomList[i].players[j]->sockfd == roomList[i].host) {
                        sprintf(resRoomResult, "%d|", hostIndex);
                        break;
                    }
                    hostIndex++;
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
                if (roomList[i].players[j] != NULL)
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
                if (roomList[i].players[j] != NULL)
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
                if (roomList[i].players[j] != NULL)
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
                if (roomList[i].players[j] != NULL)
                    if (strcmp(playerToKick, roomList[i].players[j]->name) == 0) {
                        roomList[i].host = roomList[i].players[j]->sockfd;
                        sprintf(res, "00|#|You are the host now!|");
                        send(roomList[i].players[j]->sockfd, res, strlen(res), 0);
                        sleep(0.5);
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

void initGame(int roomID) {
    for (int i = 0; i < MAX_ROOM; i++) {
        if (roomList[i].id == roomID) {
            for (int j = 0; j < MAX_PLAYER_IN_ROOM; j++) {
                if (roomList[i].players[j] != NULL) {
                    roomList[i].players[j]->state = 5;
                    roomList[i].players[j]->lose = 0;
                    roomList[i].players[j]->point = 10000;
                    roomList[i].players[j]->bet = 0;

                    roomList[i].players[j]->cards[0] = -1;
                    roomList[i].players[j]->cards[1] = -1;
                    roomList[i].players[j]->cards[2] = -1;

                    roomList[roomID].pointPool += 10000; 
                }
            }
        };
    }
}

int startGame(int curr_player_sd, int roomID) {
    int count = 0;

    for (int i = 0; i < MAX_ROOM; i++) {
        if (roomList[i].id == roomID) {
            if (curr_player_sd != roomList[i].host) return 0;     // Khong phai chu phong
            for (int j = 0; j < MAX_PLAYER_IN_ROOM; j++) {
                if (roomList[i].players[j] != NULL)
                    if(roomList[i].players[j]->state < 4) return 0;
                    else count++;
            }
            if (count < 2) return 2;
            initGame(roomID);
            return 1;           // Tat ca deu ready
            break;
        };
    }
    return 0;       // Khong tim thay phong
}

char* getResGame(int roomID) {
    char* resGameResult = (char*)calloc(1025, sizeof(char));
    int hostIndex = 0;

    for (int i = 0; i < MAX_ROOM; i++) {        // Get host
        if (roomList[i].id == roomID) {
            for (int j = 0; j < MAX_PLAYER_IN_ROOM; j++) {
                if (roomList[i].players[j] != NULL) {
                    if (roomList[i].players[j]->sockfd == roomList[i].host) {
                        sprintf(resGameResult, "%d|", hostIndex);
                        break;
                    }
                    hostIndex++;
                }
            }
            break;
        };
    }

    for (int i = 0; i < MAX_ROOM; i++) {
        if (roomList[i].id == roomID) {
            for (int j = 0; j < MAX_PLAYER_IN_ROOM; j++) {
                if (roomList[i].players[j] != NULL) {
                    sprintf(resGameResult, "%s%s|%d|%d|%d|", resGameResult, roomList[i].players[j]->name, roomList[i].players[j]->point, roomList[i].players[j]->bet, roomList[i].players[j]->lose);
                }
            }
        }
    }

    char *tmpResult = malloc(strlen(resGameResult) + 1);
    strcpy(tmpResult, resGameResult);
    return tmpResult;
}

int distributeCardRoom(int curr_player_sd, int roomID) {
    int roomIndex;
    char* res = (char*)calloc(1025, sizeof(char));

    for (int i = 0; i < MAX_ROOM; i++) {
        if (roomList[i].id == roomID) {
            roomIndex = i;
            if (curr_player_sd != roomList[i].host)
                return 0;     // Khong phai chu phong
        };
    }

    int *cards = distributeCard(countPlaying(roomID)), j = 0;;
    for (int i = 0; i < MAX_PLAYER_IN_ROOM; i++) {
        if (roomList[roomIndex].players[i] != NULL)
            if (roomList[roomIndex].players[i]->lose == 0) {
                // Moi nguoi 3 la bai
                roomList[roomIndex].players[i]->cards[0] = cards[j*3];
                roomList[roomIndex].players[i]->cards[1] = cards[j*3 + 1];
                roomList[roomIndex].players[i]->cards[2] = cards[j*3 + 2];
                roomList[roomIndex].players[i]->state = 6;
                sprintf(res, "00|#|Your cards: %s, %s, %s|", getCardName(roomList[roomIndex].players[i]->cards[0]), getCardName(roomList[roomIndex].players[i]->cards[1]), getCardName(roomList[roomIndex].players[i]->cards[2]));
                send(roomList[roomIndex].players[i]->sockfd, res, strlen(res), 0);
                j++;
            }
    }

    return 1;
}

int betPoint(int curr_player_sd, int roomID, int amount) {
    char* res = (char*)calloc(1025, sizeof(char));

    if (amount < 1000) {
        sprintf(res, "00|#|Bet point must be at least 1000.|");
        send(curr_player_sd, res, strlen(res), 0);
        return 0;
    }

    for (int j = 0; j < MAX_PLAYER_IN_ROOM; j++) {
        if (roomList[roomID].players[j]->sockfd == curr_player_sd) {
            if (roomList[roomID].players[j]->lose == 1) {
                sprintf(res, "00|#|You are loser :D|");
                send(curr_player_sd, res, strlen(res), 0);
                return 0;
            }

            if (amount > roomList[roomID].players[j]->point) {
                sprintf(res, "00|#|Bet point must be less than your total point.|");
                send(curr_player_sd, res, strlen(res), 0);
                return 0;
            }

            if (roomList[roomID].players[j]->state < 6) {
                sprintf(res, "00|#|Need to receive cards first.|");
                send(curr_player_sd, res, strlen(res), 0);
                return 0;
            }

            roomList[roomID].players[j]->bet = amount;
            roomList[roomID].players[j]->point -= amount;
            roomList[roomID].players[j]->state = 7;
            return 1;
        }
    }
}

int checkAllBetted(int roomID) {
    for (int i = 0; i < MAX_PLAYER_IN_ROOM; i++) {
        if (roomList[roomID].players[i] != NULL)
            if (roomList[roomID].players[i]->lose != 1 && roomList[roomID].players[i]->state != 7) return 0;
    }

    return 1;
}

int resetBetValue(int roomID) {
    for (int i = 0; i < MAX_PLAYER_IN_ROOM; i++) {
        if (roomList[roomID].players[i] != NULL) roomList[roomID].players[i]->bet = 0;
    }
}

int getAndResetBetValue(int roomID) {
    int result= 0;

    for (int i = 0; i < MAX_PLAYER_IN_ROOM; i++) {
        if (roomList[roomID].players[i] != NULL) {
            result += roomList[roomID].players[i]->bet;
            roomList[roomID].players[i]->bet = 0;
        }
    }

    return result;
}

int resetState(int roomID) {
    for (int i = 0; i < MAX_PLAYER_IN_ROOM; i++) {
        if (roomList[roomID].players[i] != NULL) roomList[roomID].players[i]->state = 5;
    }
}

void checkLose(int roomID) {
    for (int i = 0; i < MAX_PLAYER_IN_ROOM; i++) {
        if (roomList[roomID].players[i] != NULL) 
            if (roomList[roomID].players[i]->point < 1000) roomList[roomID].players[i]->lose = 1;
    }
}

int endGame(int roomID, int winnerIndex) {
    char* mes = (char*)calloc(1025, sizeof(char));

    for (int i = 1; i < MAX_PLAYER_IN_ROOM; i++) {
        if (roomList[roomID].players[i] != NULL)
            roomList[roomID].players[i]->state = 8;
    }

    sprintf(mes, "00|#|Final winner: %s|", roomList[roomID].players[winnerIndex]->name);
    sendChatAndNotify(roomID, mes);
    return 0;
}

int summaryRound(int roomID) {
    int highestIndex = 0;
    char* mes = (char*)calloc(1025, sizeof(char));

    for (int i = 1; i < MAX_PLAYER_IN_ROOM; i++) {
        if (roomList[roomID].players[i] != NULL)
            if (comparePoint(roomList[roomID].players[i]->cards, roomList[roomID].players[highestIndex]->cards) == 1) highestIndex = i;
    }

    roomList[roomID].players[highestIndex]->point += getAndResetBetValue(roomID);
    resetState(roomID);
    checkLose(roomID);

    sprintf(mes, "00|#|Round winner: %s - %s, %s, %s", roomList[roomID].players[highestIndex]->name, getCardName(roomList[roomID].players[highestIndex]->cards[0]), getCardName(roomList[roomID].players[highestIndex]->cards[1]), getCardName(roomList[roomID].players[highestIndex]->cards[2]));
    sendChatAndNotify(roomID, mes);

    sleep(1);

    if (roomList[roomID].players[highestIndex]->point > (roomList[roomID].pointPool)/2) {
        endGame(roomID, highestIndex);
        return 1;
    } else return 0;
}