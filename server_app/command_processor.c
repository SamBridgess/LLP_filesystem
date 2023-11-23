#include <stdio.h>
#include <stdlib.h>

int main() {
    //char directory[1000] = "../fs/mount_test";
    chdir("../fs/mount_test");


    while(1) {
        char command[1000];
        char directory[1000] = ".";
        // Get the directory path
       //  printf("Enter the directory path: ");
       // scanf("%s", directory);

        // Get the command
        printf("Enter the command: ");
        scanf(" %[^\n]s", command);


    

  // Change to the specified directory
    if(command[0] == 'c' && command[1] == 'd' && command[2] == ' ') {
            for (size_t i = 0; i <= strlen(command) - 3; i++) {
                directory[i] = command[i + 3];
            }
        
        if (chdir(directory) != 0) {
            perror("chdir");
            exit(EXIT_FAILURE);
        }
        }
        // Execute the command in the specified directory
        if (system(command) == -1) {
            perror("system");
            exit(EXIT_FAILURE);
        }
    }
    
    

    return 0;
}