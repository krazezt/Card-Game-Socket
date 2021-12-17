#include "string.h"
#include "stdlib.h"

#include "util.h"

void emptyString(char **string) {
    char result[1025] = {'\0'};
    strcpy(*string, result);
}

int readCommandCode(char *mes) {
    char code[6] = {'\0'};

    for (int i = 0; i < strlen(mes); i++) {
        if (mes[i] == '|') break;
        else code[i] = mes[i];
    }

    return atoi(code);
}

int readRoomID(char *mes) {
    char id[6] = {'\0'};
    int part = 0, x = 0;;

    for (int i = 0; i < strlen(mes); i++) {
        if (mes[i] == '|' || mes[i] == '\n' || mes[i] == '\r') part++;
        else if (part == 1) id[x++] = mes[i];
        else if (part > 1) break;
    }

    return atoi(id);
}

char* readPlayerName(char *mes) {
    char name[11] = {'\0'}, *result;
    result = calloc(11, sizeof(char));
    int part = 0, x = 0;

    for (int i = 0; i < strlen(mes); i++) {
        if (mes[i] == '|' || mes[i] == '\n' || mes[i] == '\r') part++;
        else if (part == 2) name[x++] = mes[i];
        else if (part > 2) break;
    }

    strcpy(result, name);
    return result;
}