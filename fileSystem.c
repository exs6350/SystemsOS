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

	// Set the location for the read buffer which is used to pass
	// data back to the user from the file system
	// Also, I have no idea where this is, but it is different than the fs
	sfs_read_buffer* readBuf = &fileSystem->read_buffer;
	readBuf->start = 0x30000000;
	readBuf->size = 0;
	readBuf->buf = (void *) readBuf->start;
	//fileSystem->read_buffer.start = (void *)0x4000000;
	//fileSystem->read_buffer->size = 0;
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
	uint8_t* data = 0;
	for(int i = 0; i < NUM_ENTRIES; ++i) {
		sfs_file *entry = &fileSystem->files[i];
		// check for the right file
		if(hashCommand(filename) != hashCommand((char*)&entry->filename)) 
			continue;
		
		// If there is no data here, return nothing
		if(entry->payload == 0) return data;

		// Allocate a block for the read
		//data = * to block of size size_of(unint8_t) * entry->size
		

		sfs_read_buffer* read_buffer = &fileSystem->read_buffer;
		sfs_data* source = &fileSystem->blocks[entry->payload];
		uint16_t totalToRead = entry->size;
		while(source) {
			uint16_t readSize = totalToRead;
			if(readSize > DATA_BLOCK_SIZE - sizeof(uint8_t)) readSize = DATA_BLOCK_SIZE - sizeof(uint8_t);
			// Copy the data out of the filesystem
			for( int i=0; i < readSize; ++i ) {
				read_buffer->buf = source->data[i];
				read_buffer->size++;
				read_buffer->buf++;
			}
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
		
		// Check for the right file
		if(hashCommand(filename) != hashCommand((char*)&entry->filename)) 
			continue;
		
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
			for(int i = 0; i < writeSize; ++i ) {
				destination->data[i] = (uint8_t) buffer;
				buffer++;
			}
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

// Will this function print the files here? Or should we return some pointer to the list of files?
	return (uint8_t*)1;
}

sfs_file_table* _get_fileSystem( void ) {
	return fileSystem;
}
