#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <time.h>


#define PORT 21
#define BUFFER_SIZE 1024
#define LIST_BUFFER_SIZE 8192

void handle_ftp_commands();

char* get_ls(){
    FILE *ls_output;
    char buffer[4096] = {0};

    //ls -l | sed '1d'
    //pipe
    ls_output = popen("ls -go | sed '1d'", "r");

    size_t bytesRead = fread(buffer, 1, sizeof(buffer), ls_output);

    buffer[bytesRead] = '\0';

   // for(int i = 0; i < strlen(buffer); i++)
   //     if(buffer[i] == '\t') buffer[i] = ' ';
    char* result = strdup(buffer);
    printf("%s", result);


    char b[4095];
    snprintf(b, sizeof(b), "-rw------- 1 peter 848 2023-12-20 11:22 00README.txt\r\n");
    //    snprintf(b, sizeof(b), "-rw-------  1 peter         848 дек 14 11:22 00README.txt\r\n");
    char* r = strdup(b);

    return r;
    //return result;
}
char* get_file_line(const char *filename, const struct stat *file_stat) {
    char line[256] = {0};

    char c1 = (S_ISDIR(file_stat->st_mode)) ? 'd' : '-';
    char c2 = (file_stat->st_mode & S_IRUSR) ? 'r' : '-';
    char c3 = (file_stat->st_mode & S_IWUSR) ? 'w' : '-';
    char c4 = (file_stat->st_mode & S_IXUSR) ? 'x' : '-';
    char c5 = (file_stat->st_mode & S_IRGRP) ? 'r' : '-';
    char c6 = (file_stat->st_mode & S_IWGRP) ? 'w' : '-';
    char c7 = (file_stat->st_mode & S_IXGRP) ? 'x' : '-';
    char c8 = (file_stat->st_mode & S_IROTH) ? 'r' : '-';
    char c9 = (file_stat->st_mode & S_IWOTH) ? 'w' : '-';
    char c10 = (file_stat->st_mode & S_IXOTH) ? 'x' : '-';

    char time[20];
    strftime(time, sizeof(time), "%Y-%m-%d %H:%M", localtime(&file_stat->st_mtime));

    snprintf(line, sizeof(line), "%c%c%c%c%c%c%c%c%c%c %lu %d %lld %s %s\n",
             c1, c2, c3, c4, c5, c6, c7, c8, c9, c10,
             (unsigned long)file_stat->st_nlink,
             file_stat->st_uid,
             (long long)file_stat->st_size,
             time,
             filename);

    return strdup(line);
}
char* get_listing(){
    const char *directory_path = ".";
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;

    dir = opendir(directory_path);
    if (dir == NULL) {
        perror("Error opening directory");
        return "Error opening directory";
    }

    char info[LIST_BUFFER_SIZE] = {0};
    while ((entry = readdir(dir)) != NULL) {
        char file_path[256];
        snprintf(file_path, sizeof(file_path), "%s/%s", directory_path, entry->d_name);

        if (stat(file_path, &file_stat) == -1) {
            perror("Error getting file information");
            continue;
        }

        char* line = get_file_line(entry->d_name, &file_stat);
        strcat(info, line);
        free(line);
    }
    printf("Listing:\n%s", info);
    closedir(dir);
    char *l = strdup(info);


    return l;
}

void send_response(int socket, const char *response) {
    send(socket, response,  strlen(response), 0);
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

const char* get_server_ip_address(int control_socket) {
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    getsockname(control_socket, (struct sockaddr*)&addr, &addr_len);
    return inet_ntoa(addr.sin_addr);
}
int get_data_port_high(int data_socket) {
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    getsockname(data_socket, (struct sockaddr*)&addr, &addr_len);
    return ntohs(addr.sin_port) / 256;
}
int get_data_port_low(int data_socket) {
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    getsockname(data_socket, (struct sockaddr*)&addr, &addr_len);
    return ntohs(addr.sin_port) % 256;
}

int open_passive_data_socket(char *ip, int *port) {
    int data_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (data_socket == -1) {
        perror("Error creating data socket");
        return -1;
    }

    struct sockaddr_in data_server_addr;
    memset(&data_server_addr, 0, sizeof(data_server_addr));
    data_server_addr.sin_family = AF_INET;
    data_server_addr.sin_addr.s_addr = INADDR_ANY;
    //data_server_addr.sin_addr.s_addr =  inet_addr("127.0.0.1");


    data_server_addr.sin_port = 0;

    //bind the data socket to the local address and port
    if (bind(data_socket, (struct sockaddr*)&data_server_addr, sizeof(data_server_addr)) == -1) {
        perror("Error binding data socket");
        close(data_socket);
        return -1;
    }

    //listen for incoming connections on the data socket
    if (listen(data_socket, 1) == -1) {
        perror("Error listening on data socket");
        close(data_socket);
        return -1;
    }

    //get the local address and port of the data socket
    struct sockaddr_in data_addr;
    socklen_t addr_len = sizeof(data_addr);
    if (getsockname(data_socket, (struct sockaddr*)&data_addr, &addr_len) == -1) {
        perror("Error getting data socket information");
        close(data_socket);
        return -1;
    }

    //extract the IP address and port from the sockaddr_in structure
    strcpy(ip, inet_ntoa(data_addr.sin_addr));
    *port = ntohs(data_addr.sin_port);

    return data_socket;
}
int accept_passive_connection(int passive_socket) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Accept a connection on the passive socket
    int data_socket = accept(passive_socket, (struct sockaddr*)&client_addr, &addr_len);
    if (data_socket == -1) {
        perror("Error accepting connection on passive socket");
        return -1;
    }

    char client_ip[INET_ADDRSTRLEN] = {0};
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    printf("Accepted connection from %s:%d\n", client_ip, ntohs(client_addr.sin_port));

    return data_socket;
}

void receive_file(int data_socket, const char* file_path) {
    FILE* file = fopen(file_path, "wb");

    if (file == NULL) {
        perror("Error opening file for writing");
        return;
    }

    char buffer[BUFFER_SIZE] = {0};
    ssize_t bytes_received;

    while ((bytes_received = recv(data_socket, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
    }

    if (bytes_received == -1) {
        perror("Error receiving data");
    }

    fclose(file);
}
void send_file(int data_socket, const char* file_path) {
    FILE* file = fopen(file_path, "rb");

    if (file == NULL) {
        perror("Error opening file for reading");
        return;
    }

    char buffer[BUFFER_SIZE] = {0};
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        send(data_socket, buffer, bytes_read, 0);
    }

    fclose(file);
}
struct sockaddr_in server_addr, client_addr;
int server_socket, client_socket;

int main() {
    socklen_t addr_size;

    //create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    //server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

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
        handle_ftp_commands();

        close(client_socket);
    }

    close(server_socket);
    return 0;
}

void handle_ftp_commands() {
    char buffer[BUFFER_SIZE] = {0};
    char username[100] = {0};
    char password[100] = {0};

    send_response(client_socket, "220 Welcome to FTP server\r\n");

    int data_socket = -1;

    while (1) {
        //receive command from the client
        memset(buffer, 0, sizeof(buffer));
        if (recv(client_socket, buffer, sizeof(buffer), 0) == -1) {
            perror("Error receiving command");
            break;
        }
        char current_directory[BUFFER_SIZE] = {0};
        getcwd(current_directory, sizeof(current_directory));

        printf("Received command: %s", buffer);

        //process the command

        //ЗАТЫЧКА ДЖОКЕРА(filezilla moment)
        if (strncmp(buffer, "PORT", 4) == 0) {
            send_response(client_socket, "502 no active please =(.\r\n");
            continue;
        }

        if (strncmp(buffer, "PASV", 4) == 0) {
            char data_ip[INET_ADDRSTRLEN] = {0};
            int data_port;
            data_socket = open_passive_data_socket(data_ip, &data_port);

            char *server_ip = get_server_ip_address(client_socket);
            for(int i = 0; i < strlen(server_ip); i++)
                if(server_ip[i] == '.') server_ip[i] = ',';

            char response[BUFFER_SIZE] = {0};
            if (data_socket != -1) {
                snprintf(response, sizeof(response), "227 Entering Passive Mode (%s,%d,%d).\r\n",
                         server_ip, get_data_port_high(data_socket), get_data_port_low(data_socket));

                send_response(client_socket, response);
            } else {
                snprintf(response, sizeof(response), "425 Can't open data connection.\r\n");
                send_response(client_socket, response);
            }

            continue;
        }
        if (strncmp(buffer, "STOR", 4) == 0) {
            int accepted_connection_socket = accept_passive_connection(data_socket);
            char filename[100] = {0};
            sscanf(buffer, "STOR %s", filename);

            if (accepted_connection_socket != -1) {
                char response[BUFFER_SIZE] = {0};
                snprintf(response, sizeof(response), "150 Opening data connection for STOR.\r\n");
                send_response(client_socket, response);

                receive_file(accepted_connection_socket, filename);

                snprintf(response, sizeof(response), "226 File transfer complete.\r\n");
                send_response(client_socket, response);

                //close data connection
                close(accepted_connection_socket);
                close(data_socket);
            } else {
                char response[BUFFER_SIZE];
                snprintf(response, sizeof(response), "425 Can't open data connection.\r\n");
                send_response(client_socket, response);
            }

            continue;
        }
        if (strncmp(buffer, "RETR", 4) == 0) {
            int accepted_connection_socket = accept_passive_connection(data_socket);
            char filename[100] = {0};
            sscanf(buffer, "RETR %s", filename);

            if (accepted_connection_socket != -1) {
                char response[BUFFER_SIZE] = {0};
                snprintf(response, sizeof(response), "150 Opening data connection for RETR.\r\n");
                send_response(client_socket, response);

                send_file(accepted_connection_socket, filename);

                snprintf(response, sizeof(response), "226 File transfer complete.\r\n");
                send_response(client_socket, response);

                //close data connection
                close(accepted_connection_socket);
                close(data_socket);
            } else {
                char response[BUFFER_SIZE];
                snprintf(response, sizeof(response), "425 Can't open data connection.\r\n");
                send_response(client_socket, response);
            }

            continue;
        }
        if (strncmp(buffer, "EPSV", 4) == 0) {
            char data_ip[INET_ADDRSTRLEN] = {0};
            int data_port;
            data_socket = open_passive_data_socket(data_ip, &data_port);

            char response[BUFFER_SIZE] = {0};
            if (data_socket != -1) {
                snprintf(response, sizeof(response), "229 Entering Extended Passive Mode (|||%d|).\r\n", data_port);
                send_response(client_socket, response);
            } else {
                snprintf(response, sizeof(response), "425 Can't open data connection.\r\n");
                send_response(client_socket, response);
            }

            continue;
        }
        if (strncmp(buffer, "LIST", 4) == 0) {
            int accepted_connection_socket = accept_passive_connection(data_socket);

            if (accepted_connection_socket != -1) {
                char response[BUFFER_SIZE] = {0};
                snprintf(response, sizeof(response), "150 Opening data connection for directory listing.\r\n");
                send_response(client_socket, response);

                char listing[LIST_BUFFER_SIZE] = {0};
                char* ls_result = get_listing();

                snprintf(listing, sizeof(response), "%s\r\n", ls_result);
                send_response(accepted_connection_socket, listing);

                free(ls_result);

                snprintf(response, sizeof(response), "226 Directory listing sent.\r\n");
                send_response(client_socket, response);

                //close data connection
                close(accepted_connection_socket);
                close(data_socket);
            } else {
                char response[BUFFER_SIZE];
                snprintf(response, sizeof(response), "425 Can't open data connection.\r\n");
                send_response(client_socket, response);
            }

            continue;
        }
        if (strncmp(buffer, "TYPE", 4) == 0) {
            char transfer_mode = buffer[5];
            if (transfer_mode == 'A') {
                send_response(client_socket, "200 Switching to ASCII mode.\r\n");
            } else if (transfer_mode == 'I') {
                send_response(client_socket, "200 Switching to Binary mode.\r\n");
            } else {
                send_response(client_socket, "500 Unknown transfer mode.\r\n");
            }
            continue;
        }
        if (strncmp(buffer, "USER", 4) == 0) {
            sscanf(buffer, "USER %s", username);
            send_response(client_socket,"331 User name okay, need password\n");
            continue;
        }
        if (strncmp(buffer, "PASS", 4) == 0) {
            sscanf(buffer, "PASS %s", password);
            send_response(client_socket,"230 User logged in\n");
            continue;
        }
        if (strncmp(buffer, "RMD", 3) == 0) {
            char dir_name[100] = {0};
            sscanf(buffer, "RMD %s", dir_name);

            char dir_path[PATH_MAX] = {0};
            if (change_directory(dir_path, current_directory) == 0) {
                strcat(dir_path, "/");
                strcat(dir_path, dir_name);

                if (remove_directory(dir_path) == 0) {
                    send_response(client_socket,"250 Directory deleted\r\n");
                } else {
                    send_response(client_socket,"550 Failed to delete directory\r\n");
                }
            } else {
                send_response(client_socket,"550 Failed to delete directory\r\n");
            }
            continue;
        }
        if (strncmp(buffer, "CWD", 3) == 0) {
            char new_dir[100] = {0};
            sscanf(buffer, "CWD %s", new_dir);

            char new_dir_path[PATH_MAX] = {0};
            if (change_directory(new_dir_path, current_directory) == 0) {
                strcat(new_dir_path, "/");
                strcat(new_dir_path, new_dir);

                if (change_directory(current_directory, new_dir_path) == 0) {
                    send_response(client_socket,"250 Directory changed\r\n");
                } else {
                    send_response(client_socket,"550 Failed to change directory\r\n");
                }
            } else {
                send_response(client_socket,"550 Failed to change directory\r\n");
            }
            continue;
        }
        if (strncmp(buffer, "MKD", 3) == 0) {
            char dir_name[100] = {0};
            sscanf(buffer, "MKD %s", dir_name);

            char new_dir_path[PATH_MAX] = {0};
            if (change_directory(new_dir_path, current_directory) == 0) {
                strcat(new_dir_path, "/");
                strcat(new_dir_path, dir_name);

                if (create_directory(new_dir_path) == 0) {
                    send_response(client_socket,"257 Directory created\r\n");
                } else {
                    send_response(client_socket, "550 Failed to create directory\r\n");
                }
            } else {
                send_response(client_socket,"550 Failed to create directory\r\n");
            }
            continue;
        }
        if (strncmp(buffer, "PWD", 3) == 0) {
            char response[BUFFER_SIZE] = {0};
            snprintf(response, sizeof(response), "257 \"%s\" is the current directory\r\n", current_directory);
            send_response(client_socket, response);
            continue;
        }
        if (strncmp(buffer, "QUIT", 4) == 0) {
            send_response(client_socket, "221 Goodbye!\r\n");
            break;
        } else {
            send_response(client_socket,"500 Unknown command\r\n");
        }
    }
}

