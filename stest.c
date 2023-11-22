#include <stdlib.h>
#include <stdio.h>
#include <string.h>


  //functions to automate the creation of the filesystem and the deletion
void sOpen(){
      system("cd ")
  system("mkdir testdir");
  system("./fat -s testdir");
//  system("cd testdir");
}
void sClose(){
  system("fusermount -u testdir");
  system("rm -r testdir");
  if(system("rm fat_disk")){
    printf("End");
  }
    }


int main(int argc,char** argv){
sOpen();
int verbose = 0;
 if(argc >= 2 && strcmp(argv[1],"-v") == 0){
   
   verbose = 1;
 }
char str[80];
int count = 0;
int response = 100;
 char n[80];
 //Simply loops until mkdir runs out of memory (256 error)
 //Also added a verbose mode so one can watch as directories are made
while(response != 256){
  strcpy(str,"mkdir testdir/");
  sprintf(n, "%i", count); 
  strcat(str,n);
  if(verbose != 0){
  printf("%s\n",str);
  }
  response = system(str);
  if(verbose != 0){
  printf("%i\n",response);
  }
    if(response != 256){
      count++;
      if(verbose != 0){
      printf("%i",count);
      }
    }
 }
 printf("%s\n",n);
 sClose();
}
