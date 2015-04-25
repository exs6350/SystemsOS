/*
  File: fat.h
  Description: FAT16 definitions.
  Author: Charles R.
*/

#include "fat.h"

void init_fat16(uint8_t* ptr) {
  fat_bootsector bootsector;
  memset(&bootsector, 0, sizeof(bootsector));

  bootsector.code = { 'F', 'A', 'T' };
  bootsector.fat_copies = FAT_COPIES;
  bootsector.max_root_entries = ROOT_DIR_SIZE;

}
