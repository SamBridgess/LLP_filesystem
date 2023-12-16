#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <dirent.h>
#include <fcntl.h>

#define PORT 21
#define BUFFER_SIZE 1024

// Function declarations
void handle_ftp_commands(int client_socket);

struct sockaddr_in server_addr, client_addr;
int main() {
    int server_socket, client_socket;
    socklen_t addr_size;

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    //server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind the socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 10) == -1) {
        perror("Error listening");
        exit(EXIT_FAILURE);
    }

    printf("FTP server listening on port %d...\n", PORT);

    while (1) {
        // Accept a connection
        addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);

        if (client_socket == -1) {
            perror("Error accepting connection");
            continue;
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Handle FTP commands for each connection
        handle_ftp_commands(client_socket);

        // Close the client socket
        close(client_socket);
    }

    // Close the server socket
    close(server_socket);

    return 0;
}

void send_response(int client_socket, const char *response) {
    send(client_socket, response, strlen(response), 0);
}

void handle_ftp_commands(int client_socket) {
    char buffer[BUFFER_SIZE];
    char username[100];
    char password[100];



    // Send FTP welcome message
    send_response(client_socket, "220 Welcome to FTP server\r\n");

    while (1) {
        // Receive command from the client
        memset(buffer, 0, sizeof(buffer));
        if (recv(client_socket, buffer, sizeof(buffer), 0) == -1) {
            perror("Error receiving command");
            break;
        }

        printf("Received command: %s", buffer);
        // Process the command
        if (strncmp(buffer, "PWD", 3) == 0) {
            char current_directory[BUFFER_SIZE];
            getcwd(current_directory, sizeof(current_directory));

            char response[BUFFER_SIZE];
            snprintf(response, sizeof(response), "257 \"%s\" is the current directory\r\n", current_directory);
            send_response(client_socket, response);
            continue;
        }
        if (strncmp(buffer, "PASV", 4) == 0) {
            char* ip_address = inet_ntoa(server_addr.sin_addr);
            char comma_ip[20];
            strcpy(comma_ip, ip_address);
            for (int i = 0; i < strlen(comma_ip); i++) {
                if (comma_ip[i] == '.') {
                    comma_ip[i] = ',';
                }
            }

            // Get the port components
            int port_high = ntohs(server_addr.sin_port) / 256;
            int port_low = ntohs(server_addr.sin_port) % 256;

            char response[BUFFER_SIZE];
            snprintf(response, sizeof(response), "227 Entering Passive Mode (%s,%d,%d).\n", comma_ip, port_high, port_low);
            send_response(client_socket, response);
            continue;
        }
        if (strncmp(buffer, "LIST", 4) == 0) {
            char response[BUFFER_SIZE];





            //snprintf(response, sizeof(response), "227 Entering Passive Mode (%s,%d,%d).\n", comma_ip, port_high, port_low);

            send_response(client_socket, "directory list:\r\n");
            continue;
        }
        if (strncmp(buffer, "USER", 4) == 0) {
            sscanf(buffer, "USER %s", username);
            send_response(client_socket, "331 User name okay, need password\n");
            continue;
        }
        if (strncmp(buffer, "PASS", 4) == 0) {
            sscanf(buffer, "PASS %s", password);
            send_response(client_socket, "230 User logged in\n");
            continue;
        }
        if (strncmp(buffer, "LIST", 4) == 0) {
            send_response(client_socket, "150 Here comes the directory listing.\n");

            // Implement directory listing logic (replace with your code)
            DIR *dir;
            struct dirent *entry;

            if ((dir = opendir(".")) != NULL) {
                while ((entry = readdir(dir)) != NULL) {
                    send(client_socket, entry->d_name, strlen(entry->d_name), 0);
                    send(client_socket, "\r\n", 2, 0);
                }
                closedir(dir);
            }

            send_response(client_socket, "226 Directory send OK.\r\n");
            continue;
        }
        if (strncmp(buffer, "RETR", 4) == 0) {
            // Implement file retrieval logic (replace with your code)
            send_response(client_socket, "150 Opening data connection.\r\n");

            // Replace the following with your actual file transfer logic
            int file_fd = open("sample.txt", O_RDONLY);
            char file_buffer[BUFFER_SIZE];
            ssize_t read_size;

            while ((read_size = read(file_fd, file_buffer, sizeof(file_buffer))) > 0) {
                send(client_socket, file_buffer, read_size, 0);
            }

            close(file_fd);

            send_response(client_socket, "226 Transfer complete.\r\n");
            continue;
        }
        if (strncmp(buffer, "STOR", 4) == 0) {
            // Implement file storage logic (replace with your code)
            send_response(client_socket, "150 Opening data connection.\r\n");

            // Replace the following with your actual file transfer logic
            int file_fd = open("received_file.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
            char file_buffer[BUFFER_SIZE];
            ssize_t recv_size;

            while ((recv_size = recv(client_socket, file_buffer, sizeof(file_buffer), 0)) > 0) {
                write(file_fd, file_buffer, recv_size);
            }

            close(file_fd);

            send_response(client_socket, "226 Transfer complete.\r\n");
            continue;
        }
        if (strncmp(buffer, "QUIT", 4) == 0) {
            send_response(client_socket, "221 Goodbye!\r\n");
            break;
        } else {
            send_response(client_socket, "500 Unknown command\r\n");
        }
    }
}
