/*
  File: sfs.h
  Description: 'Shitty filesystem' declarations and other hoopla.
  Author: Charles R.
*/

#ifndef SFS_H
#define SFS_H

#include <stdint.h>

/*
  Shitty Filesystem Structure:
    512 Entries of variable length
*/

#define ENTRY_LENGTH 512
#define FILENAME_LENGTH 32

#define RAMDISK_MEM_SIZE  32 * 1024 * 1024
#define DATA_BLOCK_SIZE 512
#define NUM_BLOCKS (RAMDISK_MEM_SIZE - (ENTRY_LENGTH * sizeof(sfs_entry) + sizeof(uint8_t))) / DATA_BLOCK_SIZE


typedef struct sfs_entry {
  uint8_t filename[FILENAME_LENGTH];
  uint16_t size;
  uint16_t payload;
} sfs_entry;

typedef struct sfs_data {
  uint8_t next;
  uint8_t data[DATA_BLOCK_SIZE - sizeof(uint8_t)];
} sfs_data;

typedef struct sfs_fs {
  sfs_entry entries[ENTRY_LENGTH];
  uint8_t current_location;
  sfs_data blocks[NUM_BLOCKS];
} sfs_fs;


void sfs_init(sfs_fs* filesys);

void sfs_create(sfs_fs* filesys, char* filename);

void sfs_delete(sfs_fs* filesys, char* filename);

uint8_t* sfs_read(sfs_fs* filesys, char* filename);

void sfs_write(sfs_fs* filesys, char* filename, uint16_t size, void* buffer);

uint8_t* sfs_list(sfs_fs* filesys);

#endif
