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
	uint8_t* data = 0;
	for(int i = 0; i < NUM_ENTRIES; ++i) {
		sfs_file *entry = &fileSystem->files[i];
		// check for the right file
		//if(strcmp((char*) &entry->filename, filename))
		//	continue;

		// 
		if(entry->payload == 0) return data;

		// Allocate a block for the read
		//data = * to block of size size_of(unint8_t) * entry->size

		sfs_data* source = &fileSystem->blocks[entry->payload];
		uint16_t totalToRead = entry->size;
		while(source) {
			uint16_t readSize = totalToRead;
			if(readSize > DATA_BLOCK_SIZE - sizeof(uint8_t)) readSize = DATA_BLOCK_SIZE - sizeof(uint8_t);
			// Copy the data out of the filesystem
			totalToRead -= readSize;
			if(source->next) source = (sfs_data*) &fileSystem->blocks[source->next];
			else source = 0;
		}
		break;
	}
	return data;
}

/*
** Write to an existing file
*/
void _sfs_write(char* filename, uint16_t size, void* buffer) {
	for(int i = 0; i < NUM_ENTRIES; ++i) {
		sfs_file *entry = &fileSystem->files[i];
		
		// check for the right file
		//if(strcmp((char*) &entry->filename, filename))
		//	continue;

		// Set the starting point for this file
		if(fileSystem->current_location == -1)
			entry->payload = ++fileSystem->current_location;
		else
			entry->payload = fileSystem->current_location++;

		entry->size = size;
		sfs_data* destination = &fileSystem->blocks[entry->payload];
		uint16_t totalToWrite = size;
		while( totalToWrite ) {
			uint16_t writeSize = totalToWrite;
			if( writeSize > DATA_BLOCK_SIZE - sizeof(uint8_t))
				writeSize = DATA_BLOCK_SIZE - sizeof(uint8_t);
			// Copy the data from the buffer to our file system
			//memcpy
			totalToWrite -= writeSize;
			if(totalToWrite) destination = &fileSystem->blocks[fileSystem->current_location++];
		}
		// We have finished writing
		break;		
	}
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
