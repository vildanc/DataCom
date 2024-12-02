#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <process.h> // Ýþ parçacýklarý için

#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 12345
#define BUFFER_SIZE 1024


int generate_random_number() {
    return rand() % 100 + 1; 
}


void client_handler(void *clientSocket) {
    SOCKET clientSock = *(SOCKET *)clientSocket;
    char buffer[BUFFER_SIZE];
    int target_number = generate_random_number();
    int guess;

    printf("New game started! Target number: %d\n", target_number);

    while (1) {
        int recvSize = recv(clientSock, buffer, BUFFER_SIZE, 0);
        if (recvSize == SOCKET_ERROR || recvSize == 0) {
            printf("Client disconnected.\n");
            closesocket(clientSock);
            return;
        }

        buffer[recvSize] = '\0';
        guess = atoi(buffer);

        if (guess < target_number) {
            send(clientSock, "Higher", strlen("Higher"), 0);
        } else if (guess > target_number) {
            send(clientSock, "Lower", strlen("Lower"), 0);
        } else {
            send(clientSock, "Correct", strlen("Correct"), 0);
            break;
        }
    }

    printf("Client guessed correctly! Game over.\n");
    closesocket(clientSock);
}

int main() {
    WSADATA wsaData;
    SOCKET serverSock, clientSock;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock == INVALID_SOCKET) {
        printf("Socket creation failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

    if (bind(serverSock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind failed with error: %d\n", WSAGetLastError());
        closesocket(serverSock);
        WSACleanup();
        return 1;
    }

    if (listen(serverSock, 3) == SOCKET_ERROR) {
        printf("Listen failed with error: %d\n", WSAGetLastError());
        closesocket(serverSock);
        WSACleanup();
        return 1;
    }

    printf("Server is running on port %d...\n", SERVER_PORT);

    while (1) {
        clientSock = accept(serverSock, (struct sockaddr *)&clientAddr, &clientAddrSize);
        if (clientSock == INVALID_SOCKET) {
            printf("Accept failed with error: %d\n", WSAGetLastError());
            continue;
        }

        printf("Client connected. Starting a new thread for the client...\n");
        _beginthread(client_handler, 0, (void *)&clientSock);
    }

    closesocket(serverSock);
    WSACleanup();
    return 0;
}

