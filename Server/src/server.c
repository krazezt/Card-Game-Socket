#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

#include "room.h"
#include "util.h"

#define TRUE 1
#define FALSE 0
#define PORT 8888

extern Player playerList[MAX_PLAYER];
extern Room roomList[MAX_ROOM];

int main(int argc, char *argv[]) {
    createRoomList();
    int opt = TRUE;
    int master_socket, addrlen, new_socket, client_socket[30],
        max_clients = 30, activity, x, valread, sd;
    int max_sd, res_type;
    struct sockaddr_in address;

    char buffer[1025], res[1025], emptyStr[1025] = {'\0'};

    //set of socket descriptors
    fd_set readfds;
    Player tmpPlayer;

    //a message
    char *message = "Server connected\n";

    //initialise all client_socket[] to 0 so not checked
    for (x = 0; x < max_clients; x++) {
        client_socket[x] = 0;
    }

    //create a master socket
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Failed to create master socket!");
        exit(EXIT_FAILURE);
    }

    //set master socket to allow multiple connections ,
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    //bind the socket to localhost port 8888
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);

    //try to specify maximum of 3 pending connections for the master socket 
    if (listen(master_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");

    while (TRUE) {
        //clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        //add child sockets to set
        for (x = 0; x < max_clients; x++) {
            //socket descriptor
            sd = client_socket[x];

            //if valid socket descriptor then add to read list
            if (sd > 0)
                FD_SET(sd, &readfds);

            //highest file descriptor number, need it for the select function
            if (sd > max_sd)
                max_sd = sd;
        }

        //wait for an activity on one of the sockets , timeout is NULL ,
        //so wait indefinitely
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
        }

        //If something happened on the master socket ,
        //then its an incoming connection
        if (FD_ISSET(master_socket, &readfds)) {
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            //inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            //send new connection greeting message
            if (send(new_socket, message, strlen(message), 0) != strlen(message)) {
                perror("send");
            }

            puts("Welcome message sent successfully");

            //add new socket to array of sockets
            for (x = 0; x < max_clients; x++) {
                //if position is empty
                if (client_socket[x] == 0) {
                    client_socket[x] = new_socket;

                    //TODO: Init new player

                    Player newPlayer;
                    newPlayer.sockfd = new_socket;
                    newPlayer.state = 0;
                    playerList[x] = newPlayer;

                    //END: Init new player

                    printf("Adding to list of sockets as %d\n", x);
                    break;
                }
            }
        }

        //else its some IO operation on some other socket
        for (x = 0; x < max_clients; x++) {
            sd = client_socket[x];

            if (FD_ISSET(sd, &readfds)) {
                //Check if it was for closing , and also read the
                //incoming message
                if ((valread = read(sd, buffer, 1024)) == 0) {
                    //Somebody disconnected , get his details and print
                    getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    //TODO: Player disconnected

                    

                    //END: Player disconnected

                    //Close the socket and mark as 0 in list for reuse
                    close(sd);
                    client_socket[x] = 0;
                }

                //Echo back the message that came in
                else {
                    buffer[valread] = '\0';
                    //TODO: Received <buffer> from client <sd>, process this <buffer> message.

                    strcpy(res, emptyStr);
                    int k;

                    for (int i = 0; i < 30; i++) {
                        if (playerList[i].sockfd == sd) {
                            k = i;
                            break;
                        }
                    }

                    switch (readCommandCode(buffer)) {
                        case 2:
                            strcpy(playerList[k].name, readPlayerName(buffer));
                            playerList[k].roomID = readRoomID(buffer);
                            addPlayer(playerList[k].roomID, &(playerList[k]));
                            sprintf(res, "Joined room, roomid: %d, playerid: %d, playername: %s\n", playerList[k].roomID, playerList[k].sockfd, playerList[k].name);
                            res_type = 1;   //Send the response to all players in room.
                            break;
                        case 3:
                            tmpPlayer = findPlayer(sd);
                            removePlayer(tmpPlayer.roomID, sd);
                            sprintf(res, "Player removed\n");
                            break;
                        default:
                            break;
                    }

                    //END.

                    //TODO: Send the <res> to client(s) <sd>
                    
                    send(sd, res, strlen(res), 0);
                    
                    //END
                }
            }
        }
    }

    return 0;
}