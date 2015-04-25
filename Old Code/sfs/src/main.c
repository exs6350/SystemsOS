/*
  File: main.c
  Description: 'Shitty filesystem' tester
  Author: Charles R.
*/

#include "sfs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AUTOMATED 0
#define MAX_LINE 80

void automate(sfs_fs* filesys) {
  printf("Initializing sfs...\n");
  sfs_init(filesys);

  printf("\nListing...\n");
  sfs_list(filesys);

  printf("\nCreating a file...\n");
  sfs_create(filesys, "hello.txt");
  
  printf("\nListing...\n");
  sfs_list(filesys);

  printf("\nCreating a file...\n");
  sfs_create(filesys, "world.txt");
  
  printf("\nListing...\n");
  sfs_list(filesys);

  printf("\nDeleting a file...\n");
  sfs_delete(filesys, "world.txt");
  
  printf("\nListing...\n");
  sfs_list(filesys);

  printf("\nWriting to a file...\n");
  sfs_write(filesys, "hello.txt", 4, "abc");

  printf("\nReading from a file...\n");
  uint8_t* buffer = sfs_read(filesys, "hello.txt");
  if(buffer) printf("%s\n", buffer);
  free(buffer);
}

int main(void) { 
  sfs_fs* filesys = (sfs_fs*) malloc(sizeof(sfs_fs));
  if(AUTOMATED) {
    automate(filesys);
  } else {
    sfs_init(filesys);
    char line[MAX_LINE];
    printf("$ ");
    while(fgets(line, MAX_LINE, stdin) != NULL) {
      line[strlen(line) - 1] = '\0';
      if(strncmp(line, "exit", strlen("exit")) == 0) {
        goto end; //OH SH-
      } 

      else if(strncmp(line, "ls", strlen("ls")) == 0) {
        sfs_list(filesys);
      } 

      else if(strncmp(line, "touch ", strlen("touch ")) == 0) {
        sfs_create(filesys, &line[strlen("touch ")]);
      } 

      else if(strncmp(line, "rm ", strlen("rm ")) == 0) {
        sfs_delete(filesys, &line[strlen("rm ")]);
      } 

      else if(strncmp(line, "cat ", strlen("cat ")) == 0) {
        uint8_t* buffer = sfs_read(filesys, &line[strlen("cat ")]);
        if(buffer) printf("%s\n", buffer);
        free(buffer);
      } 

      else if(strncmp(line, "put ", strlen("put ")) == 0) {
        char* trimmed = &line[strlen("put ")];   
        char* payload = strchr(trimmed, ' ') + 1;
        *(payload - 1) = '\0';
        sfs_write(filesys, trimmed, strlen(payload), payload);       
      }
      printf("$ ");
    }
  }
  end:
  free(filesys);
  return 0;
}
