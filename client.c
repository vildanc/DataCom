#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET clientSock;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];
    char guess[BUFFER_SIZE];

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    clientSock = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSock == INVALID_SOCKET) {
        printf("Socket creation failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddr.sin_port = htons(SERVER_PORT);

    if (connect(clientSock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Connection failed with error: %d\n", WSAGetLastError());
        closesocket(clientSock);
        WSACleanup();
        return 1;
    }

    printf("Connected to the server. Start guessing the number (1-100).\n");

    while (1) {
        printf("Enter your guess: ");
        fgets(guess, BUFFER_SIZE, stdin);
        guess[strcspn(guess, "\n")] = '\0';

        if (send(clientSock, guess, strlen(guess), 0) == SOCKET_ERROR) {
            printf("Send failed with error: %d\n", WSAGetLastError());
            break;
        }

        int recvSize = recv(clientSock, buffer, BUFFER_SIZE, 0);
        if (recvSize == SOCKET_ERROR || recvSize == 0) {
            printf("Server disconnected.\n");
            break;
        }

        buffer[recvSize] = '\0';
        printf("Server: %s\n", buffer);

        if (strcmp(buffer, "Correct") == 0) {
            printf("You guessed the number!\n");
            break;
        }
    }

    closesocket(clientSock);
    WSACleanup();
    return 0;
}

