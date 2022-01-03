#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "util.h"

void emptyString(char **string) {
    char result[1025] = {'\0'};
    strcpy(*string, result);
}

char* readPart(char *mes, int partNumber) {
    char tmpStr[1025] = {'\0'}, *result;
    int part = 0, x = 0;

    for (int i = 0; i < strlen(mes); i++) {
        if (mes[i] == '|' || mes[i] == '\n' || mes[i] == '\r') part++;
        else if (part == partNumber) tmpStr[x++] = mes[i];
        else if (part > partNumber) break;
    }

    result = calloc(strlen(tmpStr), sizeof(char));
    strcpy(result, tmpStr);
    return result;
}

char* addElement(char *currentRes, char *newElement) {
    char *result = calloc(strlen(currentRes) + strlen(newElement) + 2, sizeof(char)); // +2 for the '|' and null-terminator
    strcpy(result, currentRes);
    strcat(result, newElement);
	strcat(result, "|");
    return result;
}

int readCommandCode(char *mes) {
    char* code = readPart(mes, 0);
    return atoi(code);
}

int readRoomID(char *mes) {
    char* id = readPart(mes, 1); 
    return atoi(id);
}

char* readPlayerName(char *mes) {
    char* name = readPart(mes, 2);
    return name;
}

char* readChatContent(char *mes) {
    return readPart(mes, 1);
}

char* readPlayerToKick(char *mes) {
    return readPart(mes, 1);
}

char* createChatAndNotify(char* name, char* content) {
    char *result = calloc(strlen(name) + strlen(content) + 6, sizeof(char));
    sprintf (result, "00|%s|%s|", name, content);

    return result;
};