// client/client.c

#include "functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <pthread.h> // For threading

#define UDP_PORT 11000  // Port for UDP broadcast listening
#define TCP_PORT 5001   // Port for TCP communication
// #define SECRET_KEY 'C' // XOR encryption key

// Function prototypes
// void xor_encrypt_decrypt(char *data, size_t len, char key);
void* receive_messages(void* args);

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed\n");
        exit(EXIT_FAILURE);
    }

    SOCKET udp_sock, sock;
    struct sockaddr_in recv_addr, server_addr;
    char server_ip[INET_ADDRSTRLEN];
    char username[32];
    char message[1024];
    socklen_t addr_len = sizeof(server_addr);

    // Create UDP socket to listen for server broadcasts
    udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sock == INVALID_SOCKET) {
        perror("UDP Socket creation failed");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_port = htons(UDP_PORT);
    recv_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind UDP socket to listen for broadcasts
    if (bind(udp_sock, (struct sockaddr *)&recv_addr, sizeof(recv_addr)) == SOCKET_ERROR) {
        perror("UDP Binding failed");
        closesocket(udp_sock);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    printf("Listening for server broadcast...\n");
    recvfrom(udp_sock, server_ip, sizeof(server_ip), 0, (struct sockaddr *)&server_addr, &addr_len);
    xor_encrypt_decrypt(server_ip, strlen(server_ip), SECRET_KEY); // Decrypt received IP
    server_ip[strlen(server_ip)] = '\0';
    printf("Found server at: %s\n", server_ip);
    closesocket(udp_sock); // Close UDP socket after receiving server IP

    // Create TCP socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        perror("TCP Socket creation failed");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(TCP_PORT);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        perror("Connection failed");
        closesocket(sock);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Prompt user for username
    printf("Enter your username: ");
    scanf("%31s", username);
    while (getchar() != '\n'); // Clear input buffer

    // Send username to server
    send(sock, username, strlen(username) + 1, 0);

    // Create thread for receiving messages
    pthread_t receive_thread;
    pthread_create(&receive_thread, NULL, receive_messages, (void *)&sock);

    // Main loop to send messages
    while (1) {
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = 0;

        if (strcmp(message, "/quit") == 0) {
            send(sock, "/quit", strlen("/quit"), 0);
            break;
        }

        if (strlen(message) > 0) {
            xor_encrypt_decrypt(message, strlen(message), SECRET_KEY); // Encrypt message
            send(sock, message, strlen(message), 0);
        }
    }

    printf("Disconnected from chat.\n");
    pthread_join(receive_thread, NULL);
    closesocket(sock);
    WSACleanup();
    return 0;
}

// Function to handle receiving messages from server
void* receive_messages(void* arg) {
    int sockfd = *((int*)arg);
    char buffer[1024];
    int valread;

    while (1) {
        valread = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (valread <= 0) {
            break;
        }
        buffer[valread] = '\0';
        printf("\033[1;32m%s\033[0m\n", buffer);
        // printf("me: ");
        fflush(stdout);
    }
    return NULL;
}



// Function for XOR encryption/decryption
// void xor_encrypt_decrypt(char *data, size_t len, char key) {
//     for (size_t i = 0; i < len; i++) {
//         data[i] ^= key;
//     }
    
// }
