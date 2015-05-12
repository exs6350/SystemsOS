/*
** File:	fileSystem.c
**
** Authors:	Max Roth, Nicholas Jenis, Ernesto Soltero
**
** Description:	Header for implementation of Simple File System
*/

#ifndef SFS_H
#define SFS_H

#include "types.h"
#include "klib.h"

//Constants

#define FILENAME_LENGTH 32
#define NUM_ENTRIES 512 //Maximum number of entries in file table

//Size of the disk we are allocating (on RAM for now)
#define DISK_SIZE 32 * 1024 * 1024 

#define DATA_BLOCK_SIZE 512
#define NUM_BLOCKS (DISK_SIZE - (NUM_ENTRIES * sizeof(sfs_file) + \
	sizeof(uint8_t))) / DATA_BLOCK_SIZE

/*
** Structure representing a file entry in SFS
*/
typedef struct sfs_file {
	uint8_t filename[FILENAME_LENGTH];
	uint16_t size;
	uint16_t payload; //This is something...
} sfs_file;

/*
** Structure representing the data of a file in SFS
*/
typedef struct sfs_data {
	uint8_t next;
	uint8_t data[DATA_BLOCK_SIZE - sizeof(uint8_t)];
} sfs_data;


typedef struct sfs_read_buffer {
	uint32_t start;	
	uint8_t size;
	uint32_t buf;
} sfs_read_buffer;


/*
** Structure representing the SFS file table and underlying structure
*/
typedef struct sfs_file_table {
	sfs_file files[NUM_ENTRIES];
	uint8_t current_location;
	sfs_read_buffer read_buffer;
	sfs_data blocks[NUM_BLOCKS];
} sfs_file_table;



/*
** A global pointer to the file system table
** DO NOT REFERENCE BEFORE RUNNING sfs_init();
*/
sfs_file_table* fileSystem;

/*
** Create and initialize a file system, returning it's representative
** sfs_file_table structure
*/
void _sfs_init( void );

/*
** Create a new file - do this
** Return 0 on success, anything else on error
*/
uint8_t _sfs_create(char* filename);

/*
** Delete an existing file - do this
*/
uint8_t _sfs_delete(char* filename);

/*
** Read an existing file
*/
uint8_t* _sfs_read(char* filename);

/*
** Write to an existing file
*/
void _sfs_write(char* filename, uint16_t size, void* buffer);

/*
** List all files in the directory
** Weird thing in old SFS file, no directories???
*/
uint8_t* _sfs_list( void );

/*
** Return a pointer to the file system file table
*/
sfs_file_table* _get_fileSystem( void );

#endif
