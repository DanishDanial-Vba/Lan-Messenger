# LAN Messenger Project

This is a simple LAN-based messenger application built in C, designed for communication between devices on the same local network. The project allows clients to connect to a server, exchange messages, and join group chats. The server broadcasts its IP address to clients, and the connection is secured using XOR encryption.

## Features
- **Server IP Broadcasting**: The server broadcasts its IP address over the local network using UDP, allowing clients to find and connect to the server.
- **XOR Encryption**: The server's IP address is encrypted using XOR encryption before being broadcasted, providing a basic level of obfuscation for the server's IP.
- **Client Connection**: Clients search for the server through the broadcast and establish a connection once they find it.
- **Group Chat**: After connecting, clients are prompted to enter a username and can participate in a group chat with other connected clients.

## Disclaimer
This project broadcasts the server's IP address over the local network to enable peer discovery. By running the server, users acknowledge that the server IP will be publicly visible to other devices within the same local network. The IP address is encrypted using XOR encryption during the broadcasting process for added security. However, users should still be mindful of the risks when operating in unsecured or sensitive environments.

The use of this application in public or untrusted networks is **not** recommended without further security enhancements.

## Setup and Installation

### Requirements
- C Compiler (e.g., GCC)
- Standard C library

### Installation Steps
1. Clone the repository:
    ```bash
    git clone <repository_url>
    cd <project_directory>
    ```

2. Compile the project using the Makefile:
    ```bash
    make
    ```

3. Run the server:
    ```bash
    ./server
    ```

4. Run the client:
    ```bash
    ./client
    ```

## Encryption
- **XOR Encryption**: The server's IP address is encrypted using XOR encryption before being broadcasted to the local network. This ensures the IP address is not transmitted in plain text.

**Note**: XOR encryption is a basic obfuscation technique and is not suitable for securing sensitive data. It is primarily used for protecting the server's IP address during the broadcasting process.

## Usage

- Once the server is running, clients will automatically detect the server through the broadcasted IP address and establish a connection.
- The client will be prompted to enter a **username** and can then join a **group chat** to communicate with other connected clients.

## License
This project is licensed under the GNU GENERAL PUBLIC LICENSE Version 3 - see the [LICENSE.md](LICENSE.md) file for details.
