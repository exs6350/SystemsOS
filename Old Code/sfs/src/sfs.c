/*
  File: sgs.h
  Description: 'Shitty filesystem' definitions.
  Author: Charles R.
*/

#include "sfs.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void sfs_init(sfs_fs* filesys) {
  memset(filesys, 0, sizeof(&filesys)); //Clean data is good data
  filesys->current_location = -1;
}

void sfs_create(sfs_fs* filesys, char* filename) {
  for(int i = 0; i < ENTRY_LENGTH; ++i) {
    sfs_entry* entry = &filesys->entries[i];
    if(entry->filename[0] != '\0') //Entry filled >:-(
      continue;
    int size = strlen(filename);
    if(size > 15) size = 15;
    memcpy(entry, filename, size);    
    break;
  }
}

void sfs_delete(sfs_fs* filesys, char* filename) {
  for(int i = 0; i < ENTRY_LENGTH; ++i) {
    sfs_entry* entry = &filesys->entries[i];
    if(strcmp((char*) &entry->filename, filename)) //You're not what I'm looking for >:-(
      continue;
    memset(&entry->filename, 0, sizeof(uint16_t));
    entry->size = 0;

    if(entry->payload == 0) return; //Nothing to baleet

    sfs_data* ptr = &filesys->blocks[entry->payload];
    while(ptr) {
      sfs_data* current = ptr;
      if(ptr->next == 0) ptr = NULL; else ptr = (sfs_data*) &filesys->blocks[ptr->next];
      memset(current, 0, DATA_BLOCK_SIZE);
    }
    break;
  }
}

uint8_t* sfs_read(sfs_fs* filesys, char* filename) {
  uint8_t* data = NULL;
  for(int i = 0; i < ENTRY_LENGTH; ++i) {
    sfs_entry* entry = &filesys->entries[i];
    if(strcmp((char*) &entry->filename, filename)) //You're not what I'm looking for >:-(
      continue;

    if(entry->payload == 0) return data; //Nothing to read
    
    data = (uint8_t*) malloc(sizeof(uint8_t) * entry->size);

    sfs_data* source = &filesys->blocks[entry->payload];
    uint16_t to_read = entry->size;
    while(source) {
      uint16_t amnt = to_read;
      if(amnt > DATA_BLOCK_SIZE - sizeof(uint8_t)) amnt = DATA_BLOCK_SIZE - sizeof(uint8_t);
      memcpy(data + (entry->size - to_read), source->data, amnt);
      to_read -= amnt;
      if(source->next) source = (sfs_data*) &filesys->blocks[source->next]; else source = NULL;
    }
    break;
  }
  return data;
}

void sfs_write(sfs_fs* filesys, char* filename, uint16_t size, void* buffer) {
  for(int i = 0; i < ENTRY_LENGTH; ++i) {
    sfs_entry* entry = &filesys->entries[i];
    if(strcmp((char*) &entry->filename, filename)) //You're not what I'm looking for >:-(
      continue;

    if(filesys->current_location == -1)
      entry->payload = ++filesys->current_location;
    else
      entry->payload = filesys->current_location++;
 
    entry->size = size;
    sfs_data* destination = &filesys->blocks[entry->payload];
    uint16_t to_write = size;
    while(to_write) {
      uint16_t amnt = to_write;
      if(amnt > DATA_BLOCK_SIZE - sizeof(uint8_t)) amnt = DATA_BLOCK_SIZE - sizeof(uint8_t);
      memcpy(destination->data, buffer + (size - to_write), amnt);
      to_write -= amnt;
      if(to_write) destination = &filesys->blocks[filesys->current_location++];
    }
    break;
  }
}

uint8_t* sfs_list(sfs_fs* filesys) {
  for(int i = 0; i < ENTRY_LENGTH; ++i) {
    sfs_entry* entry = &filesys->entries[i];
    if(entry->filename[0] == '\0') //Nothing here bro
      continue;
    printf("%s\n", entry->filename); //whatever
    break;
  }
  return NULL;
}
