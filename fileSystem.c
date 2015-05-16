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
#include "c_io.h"

/*
** Create and initialize a file system, returning it's representative
** sfs_file_table structure
*/
void _sfs_init( void ) {

	//Set file system to a point 1 gig into the RAM
	fileSystem = (void *)RAM_START_ADDRESS;

	//Clean the memory where we are making the file system
	_memset((uint8_t *)fileSystem, 0, sizeof(&fileSystem));
	fileSystem->current_location = 0;

	// Set the location for the read buffer which is used to pass
	// data back to the user from the file system
	// Also, I have no idea where this is, but it is different than the fs
	//sfs_read_buffer* readBuf = &fileSystem->read_buffer;
	//readBuf->start = 0x30000000;
	//readBuf->size = 0;
	//readBuf->buf = (void *) readBuf->start;
	//fileSystem->read_buffer.start = (void *)0x4000000;
	//fileSystem->read_buffer->size = 0;
}

/*
** Create a new file with one data block or empty directory
** Return 0 on success, anything else on error
*/
uint8_t _sfs_create(char* filename, uint8_t entry_type) {
	for(int i = 0; i < NUM_ENTRIES; ++i) {
		sfs_entry* entry = &fileSystem->entries[i];
		if( entry->name[0] != '\0' ) {
			continue; //Entry in use
		}
		char* c = filename;
		for(int i = 0; i < MAX_FILE_NAME_LENGTH - 1 && *c != '\0'; i++) {
			entry->name[i] = *c;
			entry->name[i+1] = '\0';
			c++;
		}
		entry->payload = fileSystem->current_location;
		entry->size = 0;
		entry->type = entry_type;
		fileSystem->current_location++;
		
		return 0;
	}
	return 1; //No open file entries
}

/*
** Delete an existing file or directory
*/
uint8_t _sfs_delete(char* filename) {
	for(int i = 0; i < NUM_ENTRIES; ++i) {
		sfs_entry* entry = &fileSystem->entries[i];
		if(compare(filename, (char*)&entry->name) != 0) {
			continue; //File entry is not what we are looking for
		}

		/*if(file->payload == 0) {
			return 0; //No data to delete, should never be the case
		}*/
		
		if(entry->type == FILE) {
			sfs_data* ptr = &fileSystem->blocks[entry->payload];
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
		}
		else if(entry->type == DIRECTORY) {
			for(int j = 0; j< NUM_ENTRIES; ++j) {
				/*if(hashCommand(filename) == hashCommand((char*)&entry->filename)) {
					continue; //File entry is not what we are looking for
				}*/
				//If file name starts with directory name, delete.
			}
		}

		_memset((uint8_t *)&entry->name, 0, sizeof(&entry->name));
		entry->name[0] = '\0';
		entry->size = 0;

		return 0;
	}
	return 1; //No file entries with that name
}

/*
** Read an existing file
*/
uint8_t* _sfs_read(char* filename) {
	uint8_t* result = 0;
	*result = 0;
	for(int i = 0; i < NUM_ENTRIES; ++i) {
		sfs_entry *entry = &fileSystem->entries[i];
		// check for the right file
		if(compare(filename, (char*)&entry->name) != 0)
			continue;
		
		// If there is no data here, return nothing
		//if(entry->payload == 0) return 0;
		//Impossible now, every file has at least one data block

		//sfs_read_buffer* read_buffer = &fileSystem->read_buffer;
		sfs_data* source = &fileSystem->blocks[entry->payload];
		
		uint16_t totalToRead = entry->size;
		uint8_t* buffer = result;
		int emptyFile = 1;
		while(source) {
			uint16_t readSize = totalToRead;
			if(readSize > DATA_BLOCK_SIZE - sizeof(uint8_t)) {
				readSize = DATA_BLOCK_SIZE - sizeof(uint8_t);
			}
			// Copy the data out of the filesystem
			for( int j=0; j < readSize; ++j ) {
				emptyFile = 0;
				*buffer = source->data[j];
				buffer++;
				//c_printf("\n%x", source->data[i]);
			}
			totalToRead -= readSize;
			if(source->next) source = (sfs_data*) &fileSystem->blocks[source->next];
			else source = 0;
		}
		// Use this for files that exist, but are empty. Using the
		// space character means that the first character in the data
		// can't be a space. We can use any other character though.
		if(emptyFile) {
			*buffer = ' ';
			buffer++;
		}
		*buffer = '\0';
		break;
	}
	//char* buf = "test";
	//return (uint8_t *) buf;
	return result;
}

/*
** Write to an existing file
*/
uint8_t _sfs_write(char* filename, uint16_t size, uint8_t* buffer) {
	
	sfs_entry *entry = _sfs_exists(filename, FILE);

	if(entry == ENTRY_DNE) {
		return -1; //Return -1 if file DNE
	}

	// Set the starting point for this file
	if(fileSystem->current_location == -1)
		entry->payload = ++fileSystem->current_location;
	else
		entry->payload = fileSystem->current_location++;

	entry->size += size;
	sfs_data* destination = &fileSystem->blocks[entry->payload];
	uint16_t totalToWrite = size;
	while( totalToWrite ) {
		uint16_t writeSize = totalToWrite;
		if( writeSize > DATA_BLOCK_SIZE - sizeof(uint8_t))
			writeSize = DATA_BLOCK_SIZE - sizeof(uint8_t);
		// Copy the data from the buffer to our file system
		for(int i = 0; i < writeSize; ++i ) {
			destination->data[i] = (uint8_t) *buffer;
			buffer++;
		}
		totalToWrite -= writeSize;
		if(totalToWrite) destination = &fileSystem->blocks[fileSystem->current_location++];
	}
	// We have finished writing
	return 0;
}

/*
** List all files in the directory
** Weird thing in old SFS file, no directories???
*/
uint8_t* _sfs_list( void ) {

	/*uint8_t* result = 0;

	uint8_t* buffer = result;
	for(int i = 0; i < NUM_ENTRIES; ++i) {
		sfs_file *entry = &fileSystem->files[i];
		if(entry->filename[0] != '\0') {
			for(int j = 0; entry->filename[j] != '\0'; j++) {
				c_puts("IN THE LOOP!");
				*buffer = entry->filename[j];
				buffer++;
				//filename++;
				//c_printf("\n%x", *filename);
			}
			*buffer = '\n';
			buffer++;
		}
	}
	*buffer = '\0';
	return result;*/
	return (uint8_t *)1;
}

/*
** Checks if a file/directory exists
**
** Returns a pointer to the file/directory if it exists
** Returns a pointer to address 512 megabytes in RAM if it does not
** 	(This only seems to work at spaces in RAM that are empty...)
*/
sfs_entry* _sfs_exists( char* filename, uint8_t filetype ) {
	for(int i = 0; i < NUM_ENTRIES; ++i) {
		sfs_entry *entry = &fileSystem->entries[i];
		
		// Check for the right file
		if(compare(filename, (char*)&entry->name) != 0 || \
				entry->type != filetype)
			continue;
		
		return entry;
	}

	//No file found;
	return ENTRY_DNE;
}

/*
** Return a pointer to the file system file table
*/
sfs_file_table* _get_fileSystem( void ) {
	return fileSystem;
}

/*
** Returns a pointer to the string holding the current directory
*/
char* get_directory( void ) {
	return directory;
}

/*
** Change the current directory
**
** Return 0 on success
** Return anything else on failure
*/
uint8_t* _set_directory( char* new_dir ) {
	//if directory exists...
		directory = new_dir;
		return 0;
	//else
		//return -1
}
