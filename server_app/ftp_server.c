#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
#include <sys/stat.h>

#define PORT 21
#define BUFFER_SIZE 1024

void handle_ftp_commands(int client_socket);
char* get_ls();
void send_response(int client_socket, const char *response);

int create_directory(const char *path);
int remove_directory(const char *path);
int change_directory(char *new_path, const char *current_path);

struct sockaddr_in server_addr, client_addr;
int main() {
    int server_socket, client_socket;
    socklen_t addr_size;

    //create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    //server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    //bind socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding");
        exit(EXIT_FAILURE);
    }

    //listen for incoming connections
    if (listen(server_socket, 10) == -1) {
        perror("Error listening");
        exit(EXIT_FAILURE);
    }

    printf("FTP server listening on port %d...\n", PORT);

    while (1) {
        addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);

        if (client_socket == -1) {
            perror("Error accepting connection");
            continue;
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        //handle FTP commands
        handle_ftp_commands(client_socket);

        close(client_socket);
    }

    close(server_socket);
    return 0;
}

void handle_ftp_commands(int client_socket) {
    char buffer[BUFFER_SIZE];
    char username[100];
    char password[100];

    send_response(client_socket, "220 Welcome to FTP server\r\n");

    while (1) {
        //receive command from the client
        memset(buffer, 0, sizeof(buffer));
        if (recv(client_socket, buffer, sizeof(buffer), 0) == -1) {
            perror("Error receiving command");
            break;
        }

        printf("Received command: %s", buffer);

        //process the command
        if (strncmp(buffer, "RMD", 3) == 0) {
            char current_directory[BUFFER_SIZE];
            getcwd(current_directory, sizeof(current_directory));

            char dir_name[100];
            sscanf(buffer, "RMD %s", dir_name);

            char dir_path[PATH_MAX];
            if (change_directory(dir_path, current_directory) == 0) {
                strcat(dir_path, "/");
                strcat(dir_path, dir_name);

                if (remove_directory(dir_path) == 0) {
                    send_response(client_socket, "250 Directory deleted\r\n");
                } else {
                    send_response(client_socket, "550 Failed to delete directory\r\n");
                }
            } else {
                send_response(client_socket, "550 Failed to delete directory\r\n");
            }
            continue;
        }
        if (strncmp(buffer, "CWD", 3) == 0) {
            char current_directory[BUFFER_SIZE];
            getcwd(current_directory, sizeof(current_directory));
            char new_dir[100];
            sscanf(buffer, "CWD %s", new_dir);

            char new_dir_path[PATH_MAX];
            if (change_directory(new_dir_path, current_directory) == 0) {
                strcat(new_dir_path, "/");
                strcat(new_dir_path, new_dir);

                if (change_directory(current_directory, new_dir_path) == 0) {
                    send_response(client_socket, "250 Directory changed\r\n");
                } else {
                    send_response(client_socket, "550 Failed to change directory\r\n");
                }
            } else {
                send_response(client_socket, "550 Failed to change directory\r\n");
            }
            continue;
        }
        if (strncmp(buffer, "MKD", 3) == 0){
            char current_directory[BUFFER_SIZE];
            getcwd(current_directory, sizeof(current_directory));

            char dir_name[100];
            sscanf(buffer, "MKD %s", dir_name);

            char new_dir_path[PATH_MAX];
            if (change_directory(new_dir_path, current_directory) == 0) {
                strcat(new_dir_path, "/");
                strcat(new_dir_path, dir_name);

                if (create_directory(new_dir_path) == 0) {
                    send_response(client_socket, "257 Directory created\r\n");
                } else {
                    send_response(client_socket, "550 Failed to create directory\r\n");
                }
            } else {
                send_response(client_socket, "550 Failed to create directory\r\n");
            }
            continue;
        }
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

            int port_high = ntohs(server_addr.sin_port) / 256;
            int port_low = ntohs(server_addr.sin_port) % 256;

            char response[BUFFER_SIZE];
            snprintf(response, sizeof(response), "227 Entering Passive Mode (%s,%d,%d).\n", comma_ip, port_high, port_low);
            send_response(client_socket, response);
            continue;
        }
        //todo LIST need fix
        if (strncmp(buffer, "LIST", 4) == 0) {
            char response[BUFFER_SIZE];

            char* ls_result = get_ls();
            snprintf(response, sizeof(response), "150 Directory listing: %s \r\n", ls_result);
            free(ls_result);
            printf("%s", response);
            send_response(client_socket, response);
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
        if (strncmp(buffer, "QUIT", 4) == 0) {
            send_response(client_socket, "221 Goodbye!\r\n");
            break;
        } else {
            send_response(client_socket, "500 Unknown command\r\n");
        }
    }
}

void send_response(int client_socket, const char *response) {
    send(client_socket, response,  strlen(response), 0);
}
int create_directory(const char *path) {
    return mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}
int remove_directory(const char *path) {
    return rmdir(path);
}
int change_directory(char *new_path, const char *current_path) {
    if (chdir(current_path) == 0) {
        if (getcwd(new_path, PATH_MAX) != NULL) return 0;
    }
    return -1;
}
char* get_ls(){
    FILE *ls_output;
    char buffer[4096];

    //pipe
    ls_output = popen("ls", "r");

    size_t bytesRead = fread(buffer, 1, sizeof(buffer), ls_output);

    buffer[bytesRead] = '\0';

    for(int i = 0; i < strlen(buffer); i++)
        if(buffer[i] == '\n') buffer[i] = ' ';
    char* result = strdup(buffer);
    printf("%s", result);

    return result;
}