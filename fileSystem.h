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

#define MAX_NAME_LENGTH 256
#define MAX_FILE_NAME_LENGTH 32
#define NUM_ENTRIES 512 //Maximum number of entries in file table

//Size of the disk we are allocating (on RAM for now)
#define DISK_SIZE 32 * 1024 * 1024 

#define DATA_BLOCK_SIZE 512
#define NUM_BLOCKS (DISK_SIZE - (NUM_ENTRIES * sizeof(sfs_entry) + \
	sizeof(uint8_t))) / DATA_BLOCK_SIZE
#define RAM_START_ADDRESS 0x40000000

//Entry types
#define DIRECTORY 1
#define FILE 2

#define ENTRY_DNE (void *)0x2000000

#define DIR_SEPERATOR '/'
#define ROOT "/"
#define CURRENT_DIR .
#define PREV_DIR ..

/*
** Structure representing a file entry in SFS
*/
typedef struct sfs_entry {
	char name[MAX_NAME_LENGTH]; //was uint8_t, no reason not to be char?
	uint16_t size; //Remove after sizeOf method is made
	uint16_t payload; //This is the first data block where data is stored
	uint8_t type;
} sfs_entry;

/*
** Structure representing the data of a file in SFS
*/
typedef struct sfs_data {
	uint8_t next;
	uint8_t data[DATA_BLOCK_SIZE - sizeof(uint8_t)];
} sfs_data;

/*
typedef struct sfs_read_buffer {
	uint32_t start;	
	uint8_t size;
	uint32_t buf;
} sfs_read_buffer;
*/

/*
** Structure representing the SFS file table and underlying structure
*/
typedef struct sfs_file_table {
	sfs_entry entries[NUM_ENTRIES];
	uint8_t current_location; //Remove once findFreeSpace is implemented
	sfs_data blocks[NUM_BLOCKS];
} sfs_file_table;



/*
** A global pointer to the file system table
** DO NOT REFERENCE BEFORE RUNNING sfs_init();
*/
sfs_file_table* fileSystem;

/*
** A global pointer to a string represetning the current directory
** DO NOT REFERENCE BEFORE RUNNING sfs_init();
*/
char* directory;

/*
** Create and initialize a file system, returning it's representative
** sfs_file_table structure
*/
void _sfs_init( void );

/*
** Create a new file - do this
** Return 0 on success, anything else on error
*/
uint8_t _sfs_create(char* name, uint8_t entry_type);

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
uint8_t _sfs_write(char* filename, uint16_t size, uint8_t* buffer);

/*
** List all files in the directory
** Weird thing in old SFS file, no directories???
*/
uint8_t* _sfs_list( void );

/*
** Checks if a file/directory exists
**
** Returns a pointer to the file/directory if it exists
** Returns a pointer to address 0 in RAM if it does not
*/
sfs_entry* _sfs_exists( char* filename, uint8_t filetype );

/*
** Return a pointer to the file system file table
*/
sfs_file_table* _get_fileSystem( void );

/*
** Returns a pointer to the string holding the current directory
*/
char* _get_directory( void );

/*
** Change the current directory
**
** Return 0 on success
** Return anything else on failure
*/
uint8_t* _set_directory( char* new_dir );

/**
** Update a filename to match the current directory
**/
char* _adjust_filename( char* filename );

#endif
