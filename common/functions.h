// common/functions.h

#ifndef FUNCTIONS_H
#define FUNCTIONS_H


#include <stddef.h>  // Required for size_t

#define SECRET_KEY 'K'  // XOR encryption key (same on client & server)

void xor_encrypt_decrypt(char *data, size_t len, char key);
void send_message(int sockfd, const char* message);
void format_message(char* message, const char* username, const char* msg_content);
int parse_message(const char* message, char* username, char* msg_content);
void receive_message(int sockfd, char* buffer, int buffer_size);

#endif
