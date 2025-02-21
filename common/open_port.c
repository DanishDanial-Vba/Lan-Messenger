#include <stdio.h>
#include <stdlib.h>

void configure_firewall() {
    system("netsh advfirewall firewall add rule name=\"LANMessenger\" dir=in action=allow protocol=UDP localport=5000");
    system("netsh advfirewall firewall add rule name=\"LANMessenger\" dir=out action=allow protocol=UDP localport=5000");
    system("netsh advfirewall firewall add rule name=\"LANMessenger\" dir=in action=allow protocol=TCP localport=5001");
    system("netsh advfirewall firewall add rule name=\"LANMessenger\" dir=out action=allow protocol=TCP localport=5001");
    printf("Firewall rules added successfully.\n");
}

int main() {
    configure_firewall();
    return 0;
}
