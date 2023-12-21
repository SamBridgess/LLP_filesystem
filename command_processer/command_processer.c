#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    chdir("../fs/mount_test");

    while(1) {
        char command[1000];
        char directory[1000] = ".";

        // Get the command
        printf("Enter the command: ");
        scanf(" %[^\n]s", command);


        // Change to the specified directory
        if(command[0] == 'c' && command[1] == 'd' && command[2] == ' ') {
            int len = strlen(command);
            for (size_t i = 0; i <= len - 3; i++) {
                directory[i] = command[i + 3];
            }
            directory[len - 1] = '\0';

            if (chdir(directory) != 0) {
                perror("chdir");
                //exit(EXIT_FAILURE);
            }
        } else {
            // Execute the command in the specified directory
            if (system(command) == -1) {
                perror("system");
                //exit(EXIT_FAILURE);
            }
        }
    }
    return 0;
}