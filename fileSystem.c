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
#include "user.h"

/*
** Create and initialize a file system, returning it's representative
** sfs_file_table structure
*/
void _sfs_init( void ) {

	//Set file system to a point 1 gig into the RAM
	fileSystem = (void *)0x40000000;

	//Clean the memory where we are making the file system
	_memset((uint8_t *)fileSystem, 0, sizeof(&fileSystem));
	fileSystem->current_location = -1;
}

/*
** Create a new file - do this
** Return 0 on success, anything else on error
*/
uint8_t _sfs_create(char* filename) {
	for(int i = 0; i < NUM_ENTRIES; ++i) {
		sfs_file* file = &fileSystem->files[i];
		if( file->filename[0] != '\0' ) {
			continue; //File entry in use
		}
		char* c = filename;
		for(int i = 0; i < FILENAME_LENGTH - 1 && *c != '\0'; i++) {
			file->filename[i] = *c;
			file->filename[i+1] = '\0';
			c++;
		}
		return 0;
	}
	return 1; //No open file entries
}

/*
** Delete an existing file - do this
*/
uint8_t _sfs_delete(char* filename) {
	for(int i = 0; i < NUM_ENTRIES; ++i) {
		sfs_file* file = &fileSystem->files[i];
		if(hashCommand(filename) != hashCommand((char*)&file->filename)) {
			continue; //File entry is not what we are looking for
		}
		_memset((uint8_t *)&file->filename, 0, sizeof(&file->filename));
		file->size = 0;

		if(file->payload == 0) {
			return 0; //No data to delete
		}

		sfs_data* ptr = &fileSystem->blocks[file->payload];
		while(ptr) {
			sfs_data* current = ptr;
			if(ptr->next == 0) {
				ptr = 0;
			}
			else {
				ptr = (sfs_data*) &fileSystem->blocks[ptr->next];
			}
			_memset((uint8_t *) current, 0, DATA_BLOCK_SIZE);
		}
		return 0;
	}
	return 1; //No file entries with that name
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
