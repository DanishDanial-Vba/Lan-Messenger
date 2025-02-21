// server.c - Dynamic Client Handling with Continuous Broadcast

#include "functions.h"
#include "get_Ip.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <pthread.h>
#include <time.h>

#define PORT 8080
#define UDP_PORT 11000
#define TCP_PORT 5001
#define BROADCAST_IP "255.255.255.255"
// #define SECRET_KEY 'C'  // XOR encryption key

// Structure for client node in linked list
typedef struct ClientNode {
    int sockfd;  // Socket descriptor for the client
    char username[32];  // Username of the client
    struct ClientNode* next;  // Pointer to the next client in the list
} ClientNode;

ClientNode* client_list = NULL;  // Head of the client list
pthread_mutex_t client_list_mutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex for thread safety

// Function prototypes
void* client_handler(void* arg);
void broadcast_message(const char* message, const char* sender_username, int sender_sockfd);
void xor_encrypt_decrypt(char *data, size_t len, char key);
void get_timestamp(char *buffer, size_t size);
void add_client(int sockfd, const char* username);
void remove_client(int sockfd);
void* broadcast_ip(void* arg);

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    SOCKET tcp_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    pthread_t broadcast_thread;
    int opt =1;
    // Create and configure the TCP socket
    tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(tcp_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(TCP_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    bind(tcp_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(tcp_sock, SOMAXCONN);
    printf("Server listening on TCP port %d...\n", TCP_PORT);

    // Start broadcasting the server IP
    
    pthread_create(&broadcast_thread, NULL, broadcast_ip, NULL);

    pthread_t tid;
    while (1) {
        int addrlen = sizeof(client_addr);
        client_sock = accept(tcp_sock, (struct sockaddr *)&client_addr, &addrlen);
     
        if (client_sock != INVALID_SOCKET) {
            int *new_sock = malloc(sizeof(int));  // Allocate memory for socket
            if (new_sock == NULL) {
                fprintf(stderr, "Memory allocation failed.\n");
                closesocket(client_sock);
                continue;
            }
            *new_sock = client_sock;  // Store client socket in allocated memory
            
            if (pthread_create(&tid, NULL, client_handler, (void*)new_sock) != 0) {
                fprintf(stderr, "Failed to create thread.\n");
                closesocket(client_sock);
                free(new_sock);
            // pthread_create(&tid, NULL, client_handler, (void*)&client_sock);
            }
        }
    }

    closesocket(tcp_sock);
    WSACleanup();
    return 0;
}

// Function to continuously broadcast the server's IP
void* broadcast_ip(void*) {
    SOCKET udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(udp_sock, SOL_SOCKET, SO_BROADCAST, (char *)&(int){1}, sizeof(int));
    struct sockaddr_in broadcast_addr;
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(UDP_PORT);
    broadcast_addr.sin_addr.s_addr = inet_addr(BROADCAST_IP);
    
    char server_ip[INET_ADDRSTRLEN];
    get_ip(server_ip, sizeof(server_ip));
    xor_encrypt_decrypt(server_ip, strlen(server_ip), SECRET_KEY);

    while (1) {
        sendto(udp_sock, server_ip, strlen(server_ip), 0, (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr));
        Sleep(5000);
    }
    closesocket(udp_sock);
    return NULL;
}

// Function to handle communication with a connected client
void* client_handler(void* arg) {
    int client_sock = *((int*)arg);
    char buffer[1024], username[32], timestamp[20];
    int valread = recv(client_sock, username, sizeof(username) - 1, 0);
    if (valread <= 0) {
        close(client_sock);
        return NULL;
    }
    username[valread] = '\0';
    printf("User %s connected.\n", username);

    add_client(client_sock, username);

    while (1) {
        valread = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
        if (valread <= 0) break;
        buffer[valread] = '\0';
        get_timestamp(timestamp, sizeof(timestamp));
        
        xor_encrypt_decrypt(buffer,strlen(buffer), SECRET_KEY);

        printf("[%s] %s: %s\n", timestamp, username, buffer);
        broadcast_message(buffer, username, client_sock);
    }

    close(client_sock);
    remove_client(client_sock);
    printf("User %s disconnected.\n", username);
    return NULL;
}

// Function to broadcast a message to all connected clients
void broadcast_message(const char* message, const char* sender_username, int sender_sockfd) {
    char formatted_message[1024];
    snprintf(formatted_message, sizeof(formatted_message), "%s: %s", sender_username, message);
    
    pthread_mutex_lock(&client_list_mutex);
    ClientNode* temp = client_list;
    while (temp) {
        if (temp->sockfd != sender_sockfd) {
            send(temp->sockfd, formatted_message, strlen(formatted_message) + 1, 0);
        }
        temp = temp->next;
    }
    pthread_mutex_unlock(&client_list_mutex);
}



// Function to add a client to the linked list
void add_client(int sockfd, const char* username) {
    pthread_mutex_lock(&client_list_mutex);
    ClientNode* new_client = (ClientNode*)malloc(sizeof(ClientNode));
    new_client->sockfd = sockfd;
    strcpy(new_client->username, username);
    new_client->next = client_list;
    client_list = new_client;
    pthread_mutex_unlock(&client_list_mutex);
}

// Function to remove a client from the linked list
void remove_client(int sockfd) {
    pthread_mutex_lock(&client_list_mutex);
    ClientNode** curr = &client_list;
    while (*curr) {
        if ((*curr)->sockfd == sockfd) {
            ClientNode* temp = *curr;
            *curr = (*curr)->next;
            free(temp);
            break;
        }
        curr = &((*curr)->next);
    }
    pthread_mutex_unlock(&client_list_mutex);
}

// Function to get a timestamp for messages
void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", localtime(&now));
}
