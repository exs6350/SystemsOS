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

	//Make sure filesystem starts out clean
	_memset((uint8_t *)fileSystem, 0, sizeof(&fileSystem));
	sfs_entry* entry;
	for(int i = 0; i < NUM_ENTRIES; i++) {
		entry = &fileSystem->entries[i];

		for(int j = 0; j < MAX_NAME_LENGTH; j++) {
			entry->name[j] = '\0';
		}		
		entry->size = 0;
		entry->payload = 0;
		entry->type = 0;
	}

	fileSystem->current_location = 0;

	sfs_data* block;
	for(int i = 0; i < NUM_BLOCKS; i++) {
		block = &fileSystem->blocks[i];

		block->next = 0;

		for(int j = 0; j < DATA_BLOCK_SIZE - sizeof(uint8_t); j++) {
			block->data[j] = 0;
		}		
	}

	for(int i = 0; i < MAX_NAME_LENGTH - MAX_FILE_NAME_LENGTH - 1; i++) {
		fileSystem->directory[i] = '\0';
	}

	_sfs_create(ROOT, DIRECTORY);
	_set_directory(ROOT);


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
uint8_t _sfs_create(char* name, uint8_t entry_type) {
	//Special use case to create the root directory, since normal creation
	//is dependant on the root existing
	if(compare(name,ROOT) == 0 && entry_type == DIRECTORY) {
		sfs_entry *entry = _sfs_exists(name, entry_type);
		if(compare((char*)&entry->name,"") == 0) {
			char* c = name;
			for(int i = 0; i < MAX_FILE_NAME_LENGTH - 1 && *c != '\0'; i++) {
				entry->name[i] = *c;
				entry->name[i+1] = '\0';
				c++;
			}
			entry->type = entry_type;
			//c_puts("ROOT CREATED!\n");
			return 7; //Return 7 on root creation
		}
		else {
			//Root already exists...
			//c_puts("ROOT DENIED!\n");
			return -7; //Return -7 on root creation denial
		}
	}

	if(len(name) <= 0) {
		return -2; //Return -2 if no entry name is specified
	}

	char* entryName = _adjust_filename(name);

	sfs_entry *entry = _sfs_exists(entryName, entry_type);	
	if(compare((char*)&entry->name,ROOT) != 0) {
		return 1; //Return 1 if the entry already exists
	}

	//Go through the adjusted filename and make sure each directory exists
	char* checkName = "";
	char* checkNameTemp = checkName;
	char* temp = entryName + 1;

	*checkNameTemp = DIR_SEPERATOR;
	checkNameTemp++;
	*checkNameTemp = '\0';

	c_puts("C:");
	c_puts(entryName);
	c_puts("|");
	c_puts(checkName);
	c_puts("\n");

	while(*temp != '\0') {
//		c_puts(temp);
//		c_puts("|");
		if(*temp == DIR_SEPERATOR) {
			*checkNameTemp = '\0';
			c_puts("\nT:");
			c_puts(temp);
			c_puts("|");
			c_puts(checkName);
			c_puts("\n");
			entry = _sfs_exists(checkName, DIRECTORY);
			if(compare((char*)&entry->name,ROOT) == 0) {
				return 2; //Return 2 if the proper directories do not exist
			}
		}
		
		*checkNameTemp = *temp;
		checkNameTemp++;
		*checkNameTemp = '\0';
		temp++;
	
	}

	for(int i = 0; i < NUM_ENTRIES; ++i) {
		sfs_entry* entry = &fileSystem->entries[i];
		if( entry->name[0] != '\0' ) {
			continue; //Entry in use
		}

		char* c = entryName;
		for(int i = 0; i < MAX_NAME_LENGTH - 1 && *c != '\0'; i++) {
			entry->name[i] = *c;
			entry->name[i+1] = '\0';
			c++;
		}

		if(entry_type == FILE) {	
			entry->payload = fileSystem->current_location;			
			fileSystem->current_location++;
		}
		else {
			entry->payload = 0;
		}

		entry->size = 0;
		entry->type = entry_type;

		return 0;
	}
	return -1; //No open file entries
}

/*
** Delete an existing file or directory
*/
uint8_t _sfs_delete(char* f_name) {

	char* filename = _adjust_filename(f_name);

	sfs_entry *entry = _sfs_exists(filename, FILE);

	if(compare((char*)&entry->name,ROOT) == 0) {
		entry = _sfs_exists(filename, DIRECTORY);
		if(compare((char*)&entry->name,ROOT) == 0) {
			return 1; //No file entries with that name
		}
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
		char* delete;
		char* nameCheck;
		sfs_entry *temp;
		int flag;
		
		//If the directory has any files/subfolders, do not delete
		for(int j = 0; j< NUM_ENTRIES; ++j) {
			temp = &fileSystem->entries[j];
			nameCheck = (char*)&temp->name;
			delete = (char*)&entry->name;
			flag = 0;

			if(len(nameCheck) > len(delete)) {
				flag = 2;
				while(*delete != '\0') {
					if(*delete != *nameCheck) {
						flag = 1;
					}
					nameCheck++;
					delete++;
				}
			}

			if(flag == 2) {
				return 2; //Return 2 if there is a file or
					  //subfolder in this directory
			}
		}
	}

	_memset((uint8_t *)&entry->name, 0, sizeof(&entry->name));
	entry->name[0] = '\0';
	entry->size = 0;

	return 0;
}

/*
** Read an existing file
*/
uint8_t* _sfs_read(char* f_name) {
	uint8_t* result = 0;
	*result = 0;

	char* filename = _adjust_filename(f_name);

	sfs_entry *entry = _sfs_exists(filename, FILE);

	if(compare((char*)&entry->name,ROOT) == 0) {
		//char* buf = "test";
		//return (uint8_t *) buf;
		return result;
	}

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
	return result;
}

/*
** Write to an existing file
*/
uint8_t _sfs_write(char* f_name, uint16_t size, uint8_t* buffer, int doAppend) {
	
	char* filename = _adjust_filename(f_name);

	sfs_entry *entry = _sfs_exists(filename, FILE);

	if(compare((char*)&entry->name,ROOT) == 0) {
		return 1; //Return 1 if file DNE
	}

	int writeSpot = 0;
	sfs_data* destination = &fileSystem->blocks[entry->payload];
	if(doAppend) {
		writeSpot = entry->size -1;
		entry->size += size;
		// Move to the right block
		// We don't actually use multiple blocks yet
		//uint8_t destination->next
	}
	else { // Overwrite what is there.
		entry->size = size;
	}
	uint16_t totalToWrite = size;
	while( totalToWrite ) {
		uint16_t writeSize = totalToWrite;
		if( writeSize + writeSpot > DATA_BLOCK_SIZE - sizeof(uint8_t))
			writeSize = DATA_BLOCK_SIZE - sizeof(uint8_t);
		// Copy the data from the buffer to our file system
		for(int i=0; i < writeSize; ++i ) {
			destination->data[writeSpot] = (uint8_t) *buffer;
			buffer++;
			writeSpot++;
		}
		writeSpot = 0;
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

	uint8_t* result = 0;
	*result = 0;

	uint8_t* buffer = result;
	for(int i = 0; i < NUM_ENTRIES; ++i) {
		sfs_entry *entry = &fileSystem->entries[i];
		if(entry->name[0] != 0) {
//			c_puts("-A");
			c_printf("\n%x\n", entry->name[0]);
			for(int j = 0; entry->name[j] != '\0'; j++) {
				*buffer = entry->name[j];
				buffer++;
			}
			*buffer = '\n';
			buffer++;
		}
	}
	*buffer = '\0';
	return result;
//	return (uint8_t *)1;
}

/*
** Checks if a file/directory exists
**
** Returns a pointer to the file/directory if it exists
** Returns a pointer to address 512 megabytes in RAM if it does not
** 	(This only seems to work at spaces in RAM that are empty...)
*/
sfs_entry* _sfs_exists( char* f_name, uint8_t filetype ) {
	
	char* filename = _adjust_filename(f_name);	

	for(int i = 0; i < NUM_ENTRIES; ++i) {
		sfs_entry *entry = &fileSystem->entries[i];
		
		// Check for the right file
		if(compare(filename, (char*)&entry->name) != 0 || \
				entry->type != filetype)
			continue;
		
		return entry;
	}

	//No file found, return root
	return &fileSystem->entries[0];
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
char* _get_directory( void ) {
	return (char*)&fileSystem->directory;
}

/*
** Change the current directory
**
** Return 0 on success
** Return anything else on failure
*/
uint8_t _set_directory( char* new_dir ) {
	if(compare(new_dir, ROOT) == 0) {
		char* c = ROOT;
		for(int i = 0; i < MAX_NAME_LENGTH - MAX_FILE_NAME_LENGTH - 2 \
				&& *c != '\0'; i++) {
			fileSystem->directory[i] = *c;
			fileSystem->directory[i+1] = '\0';
			c++;
		}

		return 0;
	}

	sfs_entry *entry = _sfs_exists(new_dir, DIRECTORY);
	if(compare((char*)&entry->name,ROOT) == 0) {
		return -1; //Directory doesn't exist
	}
	
	char* c = _adjust_filename(new_dir);
	for(int i = 0; i < MAX_NAME_LENGTH - MAX_FILE_NAME_LENGTH - 2 \
			&& *c != '\0'; i++) {
		fileSystem->directory[i] = *c;
		fileSystem->directory[i+1] = '\0';
		c++;
	}

	return 0;
}

/**
** Update a filename to match the current directory
**/
char* _adjust_filename( char* filename ) {
	
	if(*filename != DIR_SEPERATOR) {
		char* result = "NO ISSUE";
		char* tempResult = result;
		char* dir = _get_directory();
		char* tempDir = dir;
		char* tempName = filename;
		for(int i = 0; i < len(dir); i++) {
			*tempResult = *tempDir;
			tempResult++;
			tempDir++;
		}
		
		if(compare(dir, ROOT) != 0) {
			*tempResult = DIR_SEPERATOR;
			tempResult++;
		}

		for(int j = 0; j < len(filename); j++) {
			*tempResult = *tempName;
			tempResult++;
			tempName++;
		}

		*tempResult = '\0';

		return result;
	}
	else {
		return filename;
	}
}
