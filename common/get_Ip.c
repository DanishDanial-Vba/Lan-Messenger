#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stddef.h>


void get_ip(char *ip_buffer, size_t size) {
    WSADATA wsaData;
    struct addrinfo hints, *info, *p;
    char ipstr[INET_ADDRSTRLEN];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed.\n");
        return;
    }

    // Get hostname
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
        fprintf(stderr, "Error getting hostname\n");
        WSACleanup();
        return;
    }

    // Resolve hostname to IP
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname, NULL, &hints, &info) != 0) {
        fprintf(stderr, "Error getting address info\n");
        WSACleanup();
        return;
    }

    // Get first valid IP
    for (p = info; p != NULL; p = p->ai_next) {
        struct sockaddr_in *addr = (struct sockaddr_in *)p->ai_addr;
        inet_ntop(AF_INET, &addr->sin_addr, ipstr, sizeof(ipstr));
        snprintf(ip_buffer, size, "%s", ipstr);
        break;
    }

    // Cleanup
    freeaddrinfo(info);
    WSACleanup();
}
