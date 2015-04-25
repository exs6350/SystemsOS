/*
  File:        sfs.h
  Description: SFS 2.0 declarations.
  Author:      Charles R., Justin G.
  Notes:       All functions expect sanitized input, however there are some
               assertions made for basic safety. Rule of thumb: don't be stupid.
*/

#ifndef SFS_H
#define SFS_H

#include "sfs_bitmap.h"
#include "sfs_crc.h"
#include "sfs_queue.h"
#include <stdint.h>
#include <string.h>

/** Percent of disk reserved */
#define RESERVED_PERCENT 10

/** Size of the disk's 'atom' */
#define ATOM_SIZE 512

/** Maximum directory/filename length */
#define MAX_NAME_LEN 255

/* Size of the data portion of the file data block */
#define MAX_SECTOR_DATA_SIZE 500

/* Amount of padding added to the end of each directory entry */
#define ENTRY_PADDING 244

/** Directory type identifier */
#define ENTRY_DIR_TYPE 0

/* File type identifier */
#define ENTRY_FILE_TYPE 1

/* String found at the very fron of a formatted disk */
#define HEAD_STR "SFS"

/* Path delimiter token string */
#define PATH_DELIM_STR "/"

/* Path delimiter token character */
#define PATH_DELIM_CHR '/'


/** SFS Directory Entry */
typedef struct sfs_entry {
  uint8_t name[MAX_NAME_LEN];
  uint32_t length;
  uint8_t type;
  uint32_t payload;
  uint32_t next;
  uint8_t pad[ENTRY_PADDING];
} sfs_entry;

/* SFS File Data Block */
typedef struct sfs_sector {
  uint32_t next;
  uint64_t crc;
  uint8_t data[MAX_SECTOR_DATA_SIZE];
} sfs_sector;

/** SFS Disk representation */
typedef struct sfs_disk {
  uint32_t* reserved;
  uint32_t* natoms;
  sfs_entry* root;
  uint32_t* bitmap_size;
  word_t* bitmap;
} sfs_disk;


/** 
  Initialize a disk from a memory block or read in if already formatted
  Params:  size - the size of the allocated memory block
           mem  - pointer to the block of memory
  Returns: sfs_disk structure
*/
sfs_disk* sfs_init(uint32_t size, uint8_t* mem);


/** 
  Checks if a file exists on the disk.
  Params:  disk     - pointer to the disk
           filepath - filepath to check for
  Returns: TRUE on success
           FALSE on failure
*/
int sfs_exists(sfs_disk* disk, char* filepath);


/**
  Create a file on the disk
  Params:  disk     - pointer to the disk
           filepath - filepath to create
           type     - entry type (ex: directory or file type)
  Returns: TRUE on success
           FALSE on failure
*/
int sfs_create(sfs_disk* disk, char* filepath, uint8_t type);


/**
  Delete a file on the disk 
  Params:  disk     - pointer to the disk
           filepath - filepath to delete
  Returns: TRUE on success
           FALSE on failure
*/
int sfs_delete(sfs_disk* disk, char* filepath);


/**
  Gets the size of a file on the disk
  Params:  disk     - pointer to the disk
           filepath - filepath to delete
  Returns: size of file on success
           0 on directory/not found
*/
uint32_t sfs_sizeof(sfs_disk* disk, char* filepath);


/**
  Read a file on the disk
  Params:  disk     - pointer to the disk
           filepath - filepath to delete
           size     - size of the buffer to read into
           buffer   - pointer to the buffer to read into
  Returns: TRUE on success
           FALSE on failure
*/
int sfs_read(sfs_disk* disk, char* filepath, uint32_t size, void* buffer);


/**
  Write a file on the disk
  Params:  disk     - pointer to the disk
           filepath - filepath to delete
           size     - size of the buffer to write from
           buffer   - pointer to the buffer to write from
  Returns: TRUE on success
           FALSE on failure
*/
int sfs_write(sfs_disk* disk, char* filepath, uint32_t size, void* buffer);


/**
  Get a list of entries in a directory
  Params:  disk     - pointer to the disk
           filepath - filepath to list
  Returns: pointer to heap-allocated comma separated list on success
           NULL on failure
*/
char* sfs_list(sfs_disk* disk, char* filepath);

#endif
