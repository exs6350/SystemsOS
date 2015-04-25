/*
  File: fat.h
  Description: FAT16 declarations and other hoopla.
  Author: Charles R.
*/

#ifndef FAT_H
#define FAT_H

#include <stdint.h>

/*
  FAT16 FS Structure:
    Reserved/Boot
    Root
    Data
*/

#define ROOT_DIR_SIZE 512
#define FAT_COPIES 1

typedef fat_bootsector {
  uint8_t code[3]; //Code

  uint8_t os_name[8]; //OS Name

  uint16_t bytes_per_sector; //BIOS parameter block
  uint8_t sectors_per_cluster;
  uint16_t reserved_sectors;
  uint8_t fat_copies;
  uint16_t max_root_entries;
  uint16_t num_small_sectors;
  uint8_t media_descriptor;
  uint16_t sectors_per_fat;
  uint16_t sectors_per_track;
  uint16_t num_heads;
  uint32_t hidden_sectors;
  uint32_t num_large_sectors;

  uint8_t drive_number;//Ext. BIO parameter block
  uint8_t reserved;
  uint8_t ext_boot_signature;
  uint32_t vol_serial;
  uint8_t drive_number;
  uint8_t vol_label[11];
  uint8_t fs_str[8];

  uint8_t bootstrap_code[448]; //Code

  uint8_t bootsector_signature[2]; //Signature
} fat_bootsector;

typedef directory_entry {
  uint8_t filename[8];
  uint8_t filename_extension[3];
  uint8_t attribute;
  uint8_t reserved_1;
  uint8_t creation_milli;
  uint16_t creation_time;
  uint16_t creation_date;
  uint16_t last_access_date;
  uint16_t reserved_2;
  uint16_t last_write_time;
  uint16_t last_write_date;
  uint16_t start_cluster;
  uint32_t filesize;
} directory_entry;


typedef fat_fs {
  fat_bootsector bootsector;
  directory_entry[ROOT_DIR_SIZE] root;
  
} fat_fs;

inline 

void init_fat16(uint8_t* ptr);

#endif
