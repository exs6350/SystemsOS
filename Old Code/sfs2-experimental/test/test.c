/*
  File:        test.c
  Description: SFS Tester.
  Author:      Charles R.
*/


#include "sfs.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static char randchar() {
  return (char) (rand() % 95 + 32);
}

//ilu C pre-processor
#define works(msg, x) printf("%s\n", msg); if(!x) { printf("Failed! (%d)\n", x); return; } else printf("Success!\n\n");
#define PRETTY_LINE "***********************\n"
#define HEADER_LINE(x) "**   TESTING " x "   **\n"
#define printhead(x) printf(PRETTY_LINE); printf(HEADER_LINE(x)); printf(PRETTY_LINE);
#define BUFFER_SIZE 2000
#define BUFFER_FILE "/buffer.txt"
static void test_all(sfs_disk* disk) {
  srand(time(NULL));
  char* buffer = (char*) malloc(BUFFER_SIZE * sizeof(char));
  char* buffer2 = (char*) malloc(BUFFER_SIZE * sizeof(char));
  memset(buffer, 0, BUFFER_SIZE);
  memset(buffer2, 0, BUFFER_SIZE);
  for(int i = 0; i < BUFFER_SIZE; ++i) buffer[i] = randchar();

  printhead("START");

  works("Verifying root directory exists!", sfs_exists(disk, "/"));
  works("Verifying root directory's 'dot' directory exists!", sfs_exists(disk, "/."));

  works("Creating a file", sfs_create(disk, "/afile.txt", ENTRY_FILE_TYPE) == 0);
  works("Verifying file exists", sfs_exists(disk, "/afile.txt"));

  works("Creating a directory", sfs_create(disk, "/folder", ENTRY_DIR_TYPE) == 0);
  works("Verifying directory exists", sfs_exists(disk, "/folder"));

  works("Creating a file in the directory", sfs_create(disk, "/folder/asubfile1.txt", ENTRY_FILE_TYPE) == 0);
  works("Verifying file exists", sfs_exists(disk, "/folder/asubfile1.txt"));

  works("Creating a file in the directory", sfs_create(disk, "/folder/asubfile2.txt", ENTRY_FILE_TYPE) == 0);
  works("Verifying file exists", sfs_exists(disk, "/folder/asubfile2.txt"));

  works("Expecting to fail creating a file in a non-existing directory", sfs_create(disk, "/folder2/asubfile.txt", ENTRY_FILE_TYPE) != 0);

  works("Deleting a file in a root", sfs_delete(disk, "/afile.txt") == 0);
  works("Expecting to fail verifying file exists", sfs_exists(disk, "/afile.txt") == 0);

  works("Deleting a file in a directory", sfs_delete(disk, "/folder/asubfile1.txt") == 0);
  works("Expecting to fail verifying file exists", sfs_exists(disk, "/folder/asubfile1.txt") == 0);

  works("Expecting to fail deleting a non-existing file", sfs_delete(disk, "/foo.txt"));
  works("Expecting to fail deleting a non-empty directory", sfs_delete(disk, "/folder"));

  works("Writing to a test file", sfs_write(disk, BUFFER_FILE, BUFFER_SIZE, buffer) == 0);
  works("Reading back the test file", sfs_read(disk, BUFFER_FILE, BUFFER_SIZE, buffer2) == 0);
  works("Comparing read and write data", strncmp(buffer, buffer2, BUFFER_SIZE) == 0);
  works("Deleting test file", sfs_delete(disk, BUFFER_FILE) == 0);

  printf("%s\n", sfs_list(disk, "/"));
  works("Writing to a test file", sfs_write(disk, BUFFER_FILE, BUFFER_SIZE, buffer) == 0);

  printf("%s\n", sfs_list(disk, "/"));
  works("Writing to a test file (same data)", sfs_write(disk, BUFFER_FILE, BUFFER_SIZE, buffer) == 0);

  printf("%s\n", sfs_list(disk, "/"));
  for(int i = 0; i < BUFFER_SIZE; ++i) buffer[i] = randchar();
  works("Writing to a test file (different data)", sfs_write(disk, BUFFER_FILE, BUFFER_SIZE, buffer) == 0);
  printf("%s\n", sfs_list(disk, "/"));

  printhead("END  ");

  free(buffer);
  free(buffer2);
}

/*
static void dump(uint64_t size, uint8_t* memory) {
  for(int i = 0; i < size; ++i) {
    if(i && i % 4 == 0) {
      uint32_t* value = (uint32_t*) (memory + (i - 4));
      printf("\t|\t%u%s\t|\t", *value, (*value > 9999999 ? "" : "\t"));
      for(int j = 0; j < 4; ++j) {
        char ch = (char) *(memory + (i - 4 + j));
        if(ch < 32 || ch > 126) ch = '_';
        printf("%c", ch);
      }
      printf("\n");
    }
    if(i % 512 == 0) printf("\n");
    printf("0x%x\t", memory[i]);
  }
  printf("\n");
}
*/

int main(void) {
  uint64_t size = 1024 * 1024;
  uint8_t* memory = (uint8_t*) malloc(size);
  memset(memory, 0, size);

  sfs_disk* disk = sfs_init(size, memory);
  //printf("Reserved: %d\tNumber of Atoms: %d\tBitmap size: %d\n", *disk->reserved, *disk->natoms, *disk->bitmap_size);
  //dump(size, memory);

  test_all(disk);

  free(disk);
  free(memory);
  return 0;
}
