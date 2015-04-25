/*
  File:        sfs.h
  Description: SFS 2.0 definitions.
  Author:      Charles R., Justin G.
*/

#include "sfs.h"
#include <stdio.h>
#include <stdlib.h>

/******************************************************************************/
/*                     Calculations and Utilities                             */
/******************************************************************************/
#ifdef INLINE
  #define __inline__ inline
#else
  #define __inline__ 
#endif

/** Calculates the size of the bitmap need to track a number of atoms */
__inline__ uint32_t calc_bitmap_size(uint32_t natoms) {
  uint32_t nbits = natoms / CHAR_BIT;
  uint32_t nwords = (nbits / BITS_PER_WORD) + 1;
  return nwords;
}

/** Gets the offset to the first free sector */
__inline__ uint32_t find_free_sector(sfs_disk* disk) {
  return first_zero_bit(disk->bitmap);  
}

/** Checks the sector at a given offset from the root sector */
__inline__ int sector_used(sfs_disk* disk, uint32_t sector_offset) {
  return get_bit(disk->bitmap, sector_offset);
}

/** Writes an sfs_entry to the disk at a given offset from the root sector */
__inline__ void write_sector(sfs_disk* disk, sfs_entry* entry, uint32_t sector_offset) {
  memcpy(disk->root + sector_offset, entry, sizeof(sfs_entry));
  set_bit(disk->bitmap, sector_offset);
}

/** Reserves the sector at a given offset from the root sector */
__inline__ void reserve_sector(sfs_disk* disk, uint32_t sector_offset) {
  set_bit(disk->bitmap, sector_offset);
}

/** Frees the sector at a given offset from the root sector */
__inline__ void free_sector(sfs_disk* disk, uint32_t sector_offset) {
  clear_bit(disk->bitmap, sector_offset);
}


/******************************************************************************/
/*                         Private Disk Operations                            */
/******************************************************************************/

/* Prints a textual representation of an entry */
void print_entry(sfs_entry* entry) {
  printf("Name:\t\t%s\n", (char*)entry->name);
  printf("Length:\t\t%d\n", entry->length);
  printf("Type:\t\t%d\n", entry->type);
  printf("Payload address\t%d\n", entry->payload);
  printf("Next address\t%d\n", entry->next);
}

/* Prints a textual representation of a sector */
void print_sector(sfs_sector* sector) {
  printf("Next address:\t%d\n", sector->next);
  printf("CRC:\t\t0x%lx\n", sector->crc);
  printf("Data:\n");
  for(int i = 0; i < MAX_SECTOR_DATA_SIZE; ++i)
    printf("0x%x ", sector->data[i]);
  printf("\n");
}


/** Print tree recursive method */
void print_tree_rec(sfs_disk* disk, sfs_entry* entry, int depth) {
  if (entry->type == ENTRY_FILE_TYPE) {
    for(int i = 0; i < depth; i++) { printf("  "); }
    printf("%s\n", entry->name);
  } else if (entry->type == ENTRY_DIR_TYPE) {
    for(int i = 0; i < depth; i++) { printf("  "); }
    printf("%s\n", entry->name);
    if(entry != disk->root + entry->payload) { // i.e. not '.'
      print_tree_rec(disk, disk->root + entry->payload, depth + 1);
    }
  } else { //don't continue with unknown or bad data
    return; 
  }
  
  if ( entry->next != 0 ) {
    print_tree_rec(disk, disk->root + entry->next, depth);
  }
}

/** Print tree main call */
void print_tree(sfs_disk* disk) {
  printf("ROOT\n");
  //we start at depth one because no '/' entry
  print_tree_rec(disk, disk->root, 1);
}



/** Gets a queue for the tokens in a filepath, isn't destructive */
queue* get_queue(char* filepath) {
  int origlen = strlen(filepath);
  char* path = (char*) malloc(origlen);
  memcpy(path, filepath + sizeof(char), origlen); 
  queue* q = (queue*) malloc(sizeof(queue));
  queue_init(q);
  char* pch = strtok(path, PATH_DELIM_STR);
  while(pch != NULL) {
    int len = strlen(pch) + 1;
    char* token = (char*) malloc(len);
    memcpy(token, pch, len);
    queue_enqueue(q, (void*) token);
    pch = strtok(NULL, PATH_DELIM_STR);
  }
  free(path);
  return q;
}

/** Locates an entry given a path */
sfs_entry* get_entry(sfs_disk* disk, char* filepath) {
  if(filepath[0] == PATH_DELIM_CHR && filepath[1] == '\0') return disk->root;
  queue* q = get_queue(filepath);
  qnode* current_tok = q->head;
  sfs_entry* current_entry = disk->root;
  int found = 0;
  while(current_tok != NULL) {
    found = 0;
    current_entry = disk->root + current_entry->payload;
    do {
      if(strlen((char*) current_tok->data) == strlen((char*) current_entry->name) &&
         strncmp((char*) current_tok->data, (char*) current_entry->name, strlen(current_tok->data)) == 0) {
        found = 1;
        break;
      }
      if((current_entry - disk->root) == current_entry->next) //no inf loops pls
        break;
      current_entry = disk->root + current_entry->next;
    } while(current_entry != disk->root);
    if(found == 0) { 
      queue_destroy(q);
      free(q);
      return NULL;
    }
    current_tok = current_tok->next;
  }
  queue_destroy(q);
  free(q);
  return current_entry;
}

/** Removes an entry given a path */
int remove_entry(sfs_disk* disk, char* filepath) {
  if(filepath[0] == PATH_DELIM_CHR && filepath[1] == '\0') return 1; 
  queue* q = get_queue(filepath);

  void* current_tok = queue_dequeue(q);
  sfs_entry* current_entry = disk->root;
  sfs_entry* next_entry = disk->root + disk->root->next;
  int found = 0;

  while(current_tok != NULL) {
    found = 0;
    current_entry = disk->root + current_entry->payload;
    do {
      if(strlen((char*) current_tok) == strlen((char*) next_entry->name) &&
         strcmp((char*) current_tok, (char*) next_entry->name) == 0) {
        if(next_entry->type == ENTRY_FILE_TYPE || q->length == 0) {
          current_entry->next = next_entry->next;
          found = 1;
          break;
        } else {
          free(current_tok);
          current_tok = queue_dequeue(q);
          current_entry = disk->root + next_entry->payload;
          continue;
        }
      }
      if((current_entry - disk->root) == current_entry->next) //no inf loops pls
        break;
      current_entry = disk->root + current_entry->next;
      next_entry = disk->root + current_entry->next;
    } while(current_entry != disk->root);
    if(found == 0) {
      queue_destroy(q);
      free(q);
      return 1;
    }
    free(current_tok);
    current_tok = queue_dequeue(q);
  }
  queue_destroy(q);
  free(q);
  return 0;
}

/** Finds out if a directory is empty */
int is_empty_dir(sfs_disk* disk, sfs_entry* target) {
  if(target->type != ENTRY_DIR_TYPE) return 0;
  sfs_entry* payload = (sfs_entry*) (disk->root + target->payload);
  if(payload->next == 0) return 1;
  return 0;
}


/******************************************************************************/
/*                         Public Disk Operations                             */
/*                   All documentation is found in header                     */
/******************************************************************************/
sfs_disk* sfs_init(uint32_t size, uint8_t* mem) {
  uint32_t reserved = size / RESERVED_PERCENT;
  uint32_t padding = ATOM_SIZE - (reserved % ATOM_SIZE);
  reserved += padding;
  uint32_t natoms = (size - reserved) / ATOM_SIZE;
  uint32_t bitmap_size = calc_bitmap_size(natoms);

  sfs_disk* disk = NULL;
  
  if(size && mem) {
    disk = (sfs_disk*) malloc(sizeof(sfs_disk));
    uint32_t* head_ptr = (uint32_t*) (mem + sizeof(uint32_t) //align to 32bit
                                        - (strlen(HEAD_STR) % sizeof(uint32_t))); 

    const int HEAD_SIZE = strlen(HEAD_STR);

    if(strncmp((char*) mem, HEAD_STR, HEAD_SIZE) != 0) { //Am I formatted?
      memset(mem, 0, reserved); //zero out the reserved portion of the disk
      memcpy(mem, HEAD_STR, HEAD_SIZE); //write the head string

      int offset = 0;
      head_ptr[offset++] = reserved;
      head_ptr[offset++] = natoms;
      head_ptr[offset++] = bitmap_size;

      uint8_t* sector_ptr = mem + reserved;
      disk->root = (sfs_entry*) sector_ptr;

      //create a root entry and write it
      sfs_entry root;
      memset(&root, 0, ATOM_SIZE);

      memcpy(root.name, (uint8_t[MAX_NAME_LEN]){'.', '\0'}, MAX_NAME_LEN);
      root.length = 0;
      root.type = ENTRY_DIR_TYPE;
      root.next = 0;
      root.payload = 0;

      disk->bitmap = (word_t*) (head_ptr + 3);
      write_sector(disk, &root, 0);
    }

    disk->reserved = head_ptr++;
    disk->natoms = head_ptr++;
    disk->root = (sfs_entry*) (mem + *disk->reserved);
    disk->bitmap_size = head_ptr++;
    disk->bitmap = (word_t*) head_ptr;
  }
  return disk;
}

int sfs_exists(sfs_disk* disk, char* filepath) {
  return get_entry(disk, filepath) != NULL;
}


int sfs_create(sfs_disk* disk, char* filepath, uint8_t type) {
  uint32_t path_len = strlen(filepath) + 1;
  char* path = (char*) malloc(path_len);
  char* filename = (char*) malloc(path_len);

  strcpy(path, filepath);

  if(strchr(path + 1, PATH_DELIM_CHR)) {
    char* name = path + strlen(path) - 1;
    while(*(--name) != PATH_DELIM_CHR);
    *name = '\0';
    name++;
    strcpy(filename, name);
  } else {
    strcpy(filename, path + 1);
    strcpy(path, PATH_DELIM_STR);
  }

  sfs_entry* dir_target = get_entry(disk, path);

  if(dir_target == NULL || dir_target->type == ENTRY_FILE_TYPE) {
    free(path);
    free(filename); 
    return 1;
  }

  dir_target = disk->root + dir_target->payload; //work from .
  
  sfs_entry nentry;
  memset(&nentry, 0, ATOM_SIZE);

  strcpy((char*)nentry.name, filename);
  nentry.length = 0;
  nentry.type = type;
  nentry.next = dir_target->next;

  int sector_loc = find_free_sector(disk);
  dir_target->next = sector_loc;
  
  //reserve the sector ahead of time instead of allocating a pointer later
  reserve_sector(disk, sector_loc); 

  if(type == ENTRY_FILE_TYPE) {
    nentry.payload = 0;
  } else if(type == ENTRY_DIR_TYPE) {
    int selfsector_loc = find_free_sector(disk);

    sfs_entry self;
    memset(&self, 0, ATOM_SIZE);

    memcpy(self.name, (uint8_t[MAX_NAME_LEN]){'.', '\0'}, MAX_NAME_LEN);
    self.length = 0;
    self.type = ENTRY_DIR_TYPE;
    self.next = 0;
    self.payload = selfsector_loc;

    write_sector(disk, &self, selfsector_loc);
    nentry.payload = selfsector_loc;
  }

  write_sector(disk, &nentry, sector_loc);

  free(path);
  free(filename);
  return 0;
}

int sfs_delete(sfs_disk* disk, char* filepath) {
  sfs_entry* target = get_entry(disk, filepath);
  if(target == NULL) return 1;
  int rc = 0;

  if(target->type == ENTRY_FILE_TYPE) {
    if(target->payload) {
      sfs_sector* payload = (sfs_sector*) (disk->root + target->payload);
      free_sector(disk, target->payload);
      while(1) {
        if(payload->next) {
          free_sector(disk, payload->next);
          payload = (sfs_sector*) (disk->root + payload->next);
        } else break;
      }
    }
    free_sector(disk, target - disk->root);
    rc = remove_entry(disk, filepath) == 0;
  } else if(target->type == ENTRY_DIR_TYPE && is_empty_dir(disk, target)) {
    sfs_entry* payload = (sfs_entry*) (disk->root + target->payload);
    free_sector(disk, payload - disk->root);
    free_sector(disk, target - disk->root);
    rc = remove_entry(disk, filepath) == 0;
  }
  return rc;
}

uint32_t sfs_sizeof(sfs_disk* disk, char* filepath) {
  sfs_entry* entry = get_entry(disk, filepath);
  if(entry) return entry->length;
  return 0;
}

int sfs_read(sfs_disk* disk, char* filepath, uint32_t size, void* buffer) {
  sfs_entry* entry = get_entry(disk, filepath);
  if(entry == NULL || entry->type == ENTRY_DIR_TYPE) return 1;

  uint32_t offset = entry->payload;
  uint32_t read = 0;
  while(read < size && offset) {
    sfs_sector* current = (sfs_sector*) (disk->root + offset);

    if((size - read) >= MAX_SECTOR_DATA_SIZE)
      memcpy(buffer + read, current->data, MAX_SECTOR_DATA_SIZE);
    else
      memcpy(buffer + read, current->data, size - read);
    read += MAX_SECTOR_DATA_SIZE;

    offset = current->next;
  }
  return 0;
}

int sfs_write(sfs_disk* disk, char* filepath, uint32_t size, void* buffer) {
  sfs_entry* entry = get_entry(disk, filepath);

  if(!entry) { //new file, we should create it
    if(sfs_create(disk, filepath, ENTRY_FILE_TYPE)) return 1;
    entry = get_entry(disk, filepath);
  }
  if(entry->payload == 0) {
    entry->payload = find_free_sector(disk);
    reserve_sector(disk, entry->payload);
    sfs_sector* sector = (sfs_sector*) (disk->root + entry->payload);
    sector->next = 0;
    sector->crc = 0;
    memset(sector->data, 0, MAX_SECTOR_DATA_SIZE);
  }

  int next_write = entry->payload;
  entry->length = size;
  void* bufptr = buffer;
  
  while(size > 0) {
    sfs_sector* work_sector = (sfs_sector*) (disk->root + next_write);

    if(size > MAX_SECTOR_DATA_SIZE && work_sector->next == 0) {
      work_sector->next = find_free_sector(disk);
      reserve_sector(disk, work_sector->next);
    }
    next_write = work_sector->next;
    
    if(size > MAX_SECTOR_DATA_SIZE) {
      memcpy(work_sector->data, bufptr, MAX_SECTOR_DATA_SIZE);
      size -= MAX_SECTOR_DATA_SIZE;
      bufptr += MAX_SECTOR_DATA_SIZE;
    } else {
      memcpy(work_sector->data, bufptr, size);
      size = 0;
    }
    work_sector->crc = crc64(work_sector->data, MAX_SECTOR_DATA_SIZE);
  }  
  return 0;
}

char* sfs_list(sfs_disk* disk, char* filepath) {
  sfs_entry* target = get_entry(disk, filepath);
  if(target == NULL) return NULL;

  if(target->type == ENTRY_FILE_TYPE) {
    char* result = (char*) malloc(strlen((char*) target->name));
    strcpy(result, (char*) target->name);
    return result;
  } 
  else if(target->type == ENTRY_DIR_TYPE) {
    int buffer_size = 1024;
    char* result = (char*) malloc(buffer_size);
    sfs_entry* entry = (sfs_entry*) (disk->root + target->payload);
    int read = 0;
    do {
      char* name = (char*) entry->name;
      int name_len = strlen(name);
      if(name_len + read + 1 > buffer_size){
        char* newbuffer = (char*) malloc(buffer_size * 2);
        memcpy(newbuffer, result, buffer_size);
        free(result);
        result = newbuffer;
        buffer_size *= 2;
      }

      memcpy(result + read, name, name_len);
      read += name_len;
      *(result + read++) = ',';
      *(result + read) = '\0';
      entry = (sfs_entry*) disk->root + entry->next;
    } while(entry != disk->root);
    *(result + read - 1) = '\0';
    return result;
  }
  return NULL;
}
