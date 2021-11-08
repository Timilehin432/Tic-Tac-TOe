// Tic-tac-toe (T3) Network Game
// Jason Losh

//-----------------------------------------------------------------------------
// Compile notes for C99 code
//-----------------------------------------------------------------------------

// gcc -o t3 t3.c udp.c

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdlib.h>          // EXIT_ codes
#include <stdbool.h>         // bool
#include <stdio.h>           // printf, scanf
#include <string.h>          // strlen, strcmp
#include "udp.h"

#define CLIENT_PORT 4096
#define SERVER_PORT 4097

void clearBoard(char board[3][3])
{
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            board[r][c] = '.';
}
// TODO: Add code to display the game board
void showBoard(char board[3][3])
{
    char * optns = "ABC";
    printf("  1 2 3\n");
    for(int i = 0 ; i < 3; i++){
        printf("%c ", optns[i]);
        for(int j = 0 ; j < 3; j++){
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
}

bool checker(char a, char b, char c, char xo){
    if(a == ' ' || b==' ' || c==' '){
        return false;
    }
    if((a == b)  && (b== c) && ( c== xo)){
        return true;
    }
    return false;
}
// TODO: Add code to determine if a winner (xo) has occurred
bool isWinner(char board[3][3], char xo)
{
    bool win = false;
    for(int i = 0 ; i < 3; i++){
        if(checker(board[0][i],board[1][i],board[2][i], xo)){
            win = true;
        }
        if(checker(board[i][0],board[i][1],board[i][2], xo)){
            win = true;
        }
    }
    if(checker(board[0][0], board[1][1], board[2][2], xo)){
        win = true;
    }
    if(checker(board[2][0], board[1][1], board[0][2], xo)){
        win = true;
    }
    return win;
}
// TODO: Add code to test if an x or o (xo) is a valid move
//       and then record in the game board
bool addMove(char board[3][3], char move[], char xo)
{
    bool ok = strlen(move) == 2;
    int x = move[1] - '0';
    x = x -1 ;
    switch(move[0]){
        case 'A':
            if(board[0][x] !='.'){
                ok = false;
                break;
            }
            board[0][x] = xo;
            break;
        case 'B':
            if(board[1][x] !='.'){
                ok = false;
                break;
            }
            board[1][x] = xo;
            break;
        case 'C':
            if(board[2][x] !='.'){
                ok = false;
                break;
            }
            board[2][x] = xo;
            break;
        default:
            ok = false;
            break;
    }
    return ok;
}
//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    char *remoteIp;
    char *role;
    char str[100], str2[100];
    char board[3][3];
    bool validMove;
    bool winner = false;
    int moveCount = 0;
    clearBoard(board);
    bool myTurn;
    char myLetter, opponentLetter;
    int remotePort;

    // TODO: Verify arguments are valie
    bool goodArguments = false;
    if(argc == 3){
        if(strcmp(argv[2],"invite")==0 || strcmp(argv[2],"accept")==0){
           goodArguments = true; 
           role = argv[2];
           remoteIp = argv[1];
        }
    }
    // ...
    if (!goodArguments)
    {
        printf("usage: t3 IPV4_ADDRESS ROLE\n");
        printf("  where:\n");
        printf("  IPV4_ADDRESS is address of the remote machine\n");
        printf("  ROLE is either invite or accept\n");
        exit(EXIT_FAILURE);
    }

    // TODO: Determine if client or server
    // A server will wait to accept an invitation to play
    // A client will send an invitation to play
    bool client = true;
    if(strcmp(role, "accept") ==0)
        client = false;

    
    // TODO: Determine remote port that you will send data to
    int sending_port;
    char data[6];
    // TODO: Open listener port number dependent on client/server role
    if(client){
        if(!openListenerPort(remoteIp, CLIENT_PORT)){
            printf("failed exiting!\n");
            exit(0);
        };
        sending_port = SERVER_PORT;
    // TODO: Determine your letter (x or o) and your opponent's letter
    // TODO: Add code to send an invite or wait to accept an invite
        myLetter = 'x';
        opponentLetter = 'o';
        if(sendData(remoteIp, SERVER_PORT, "invite")){
            printf("invite sent! \n");
        };
    }
    else{
        if(!openListenerPort(remoteIp, SERVER_PORT)){
            printf("Failed stating server !");
            exit(0);
        }
        printf("waiting for client\n");
        receiveData(data, 6);
        while(strcmp(data, "invite") !=0){
            printf("%s\n",data);
            receiveData(data, 6);
        }
        sending_port = CLIENT_PORT;
        myLetter = 'o';
        opponentLetter = 'x';
    }
    //       If you are server, send to client port, and vice versa

    // Setup game
    
    clearBoard(board);
    // TODO: Determine whether it is your turn or not
    myTurn = !client;
    char move[2];
    showBoard(board);
    // Start game loop, alternating turns
    while(!winner && moveCount != 9)
    {
        // get my move
        if (myTurn)
        {
            // TODO: add code your move here to get the move, validate move,
            while(!addMove(board, move, myLetter)){
                printf("Enter your move : ");
                scanf("%s", move);
            };
            sendData(remoteIp, sending_port, move);
            showBoard(board);
            //       show board, and check for a winner
            if(isWinner(board,myLetter)){
                printf("you won !");
                exit(0);
            }
            printf("opponent's turn\n");
        }
        else
        {
            // TODO: add code to receive your opponent's move, validate move,
            receiveData(move, 2);
            addMove(board, move, opponentLetter);
            showBoard(board);
            //       show board, and check for a winner
            if(isWinner(board,opponentLetter)){
                printf("opponent won!");
                exit(0);
            }
        }
        // Increment move count, alternate turns
        moveCount++;
        myTurn = !myTurn;
    }
    if (!winner)
        printf("The game was a draw\n");
    // TO DO: Close listener port
    closeListenerPort();
    return EXIT_SUCCESS;
}
