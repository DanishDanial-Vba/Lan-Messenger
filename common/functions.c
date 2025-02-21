#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "functions.h"



// XOR encryption and decryption function
void xor_encrypt_decrypt(char *data, size_t len, char key) {
    for (size_t i = 0; i < len; i++) {
        data[i] ^= key;
    }
    data[len] = '\0';  // Ensure null termination
}


// Function to send an encrypted message through a socket
void send_message(int sockfd, const char* message) {
    char encrypted_message[1024];
    strncpy(encrypted_message, message, sizeof(encrypted_message) - 1);
    encrypted_message[sizeof(encrypted_message) - 1] = '\0';

    xor_encrypt_decrypt(encrypted_message, strlen(encrypted_message), SECRET_KEY);
    
    if (send(sockfd, encrypted_message, strlen(encrypted_message), 0) == -1) {
        perror("send failed");
    }
}

// Function to receive and decrypt a message from the socket
void receive_message(int sockfd, char* buffer, int buffer_size) {
    int bytes_received = recv(sockfd, buffer, buffer_size - 1, 0);
    if (bytes_received < 0) {
        perror("recv failed");
        return;
    } else if (bytes_received == 0) {
        printf("Connection closed by the server.\n");
        return;
    }

    buffer[bytes_received] = '\0'; // Null-terminate before decryption

    printf("Raw Received Data: %s\n", buffer); // Debugging

    xor_encrypt_decrypt(buffer, bytes_received, SECRET_KEY); // Decrypt

    printf("Decrypted Message: %s\n", buffer);
}




