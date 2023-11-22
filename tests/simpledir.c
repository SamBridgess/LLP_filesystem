#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main () {

   char command[50];

   strcpy( command, "mkdir testdir" );
   system( command );

   strcpy( command, "./fat -s testdir" );
   system( command );

   strcpy( command, "ls testdir" );
   system( command );

   strcpy( command, "cd testdir" );
   system( command );

   strcpy( command, "cd .." );
   system( command );

   strcpy( command, "fusermount -u" );
   system( command );

   return(0);

}
