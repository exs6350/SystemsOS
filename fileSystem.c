/*
** File:	fileSystem.c
**
** Authors:	Max Roth, Nicholas Jenis, Ernesto Soltero
**
** Description:	Implementation of Simple File System
*/

#include "types.h"
#include "fileSystem.h"
#include "klib.h"

/*
** Create and initialize a file system, returning it's representative
** sfs_file_table structure
*/
void _sfs_init( void ) {

	//Set file system to a point 1 gig into the RAM
	fileSystem = (void *)0x40000000;

	_memset((uint8_t *)fileSystem, 0, sizeof(&fileSystem));
	fileSystem->current_location = -1;
}

/*
** Create a new file
*/
void _sfs_create(char* filename) {
}

/*
** Delete an existing file
*/
void _sfs_delete(char* filename) {
}

/*
** Read an existing file
*/
uint8_t* _sfs_read(char* filename) {
	return (uint8_t*)1;
}

/*
** Write to an existing file
*/
void _sfs_write(char* filename, uint16_t size, void* buffer) {
}

/*
** List all files in the directory
** Weird thing in old SFS file, no directories???
*/
uint8_t* _sfs_list( void ) {
	return (uint8_t*)1;
}

sfs_file_table* _get_fileSystem( void ) {
	return fileSystem;
}
