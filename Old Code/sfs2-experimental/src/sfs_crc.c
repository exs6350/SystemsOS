/*
  File:        sfs_crc.c
  Description: CRC32/64 definitions.
  Author:      Charles R.
  Notes:       Special thanks to Addison-Wesley's Hacker's Delight
*/

#include "sfs_crc.h"
#include <stdlib.h>
#include <string.h>

/******************************************************************************/
/*                         Public CRC Operations                              */
/*                   All documentation is found in header                     */
/******************************************************************************/

uint32_t crc32(uint8_t* message, uint32_t length) {
  const uint32_t g0 = 0xEDB88320, g1 = g0 >> 1,
                 g2 = g0 >> 2, g3 = g0 >> 3,
                 g4 = g0 >> 4, g5 = g0 >> 5,
                 g6 = (g0 >> 6) ^ g0, 
                 g7 = ((g0 >> 6) ^ g0) >> 1;

  uint32_t offset = 0;
  int32_t crc = 0xFFFFFFFF;
  uint32_t c;
  uint8_t byte;
  while(length--) {
    byte = message[offset++];
    crc = crc ^ byte;
    c = ((crc << 31 >> 31) & g7) ^ ((crc << 30 >> 31) & g6) ^
        ((crc << 29 >> 31) & g5) ^ ((crc << 28 >> 31) & g4) ^
        ((crc << 27 >> 31) & g3) ^ ((crc << 26 >> 31) & g2) ^
        ((crc << 25 >> 31) & g1) ^ ((crc << 24 >> 31) & g0);
    crc = ((uint32_t) crc >> 8) ^ c;
  }
  return ~crc;
}

uint64_t crc64(uint8_t* message, uint32_t length) {
  if(length % 2 != 0) return 0;
  uint32_t part_length = length / 2;
  uint8_t* head = (uint8_t*) malloc(part_length);
  uint8_t* tail = (uint8_t*) malloc(part_length);
  memcpy(head, message, part_length);
  memcpy(tail, message + part_length, part_length);

  uint32_t head_crc = crc32(head, part_length);
  uint32_t tail_crc = crc32(tail, part_length);

  uint64_t crc = (((uint64_t) head_crc) << 32) + ((uint64_t) tail_crc);
  free(head);
  free(tail);
  return crc;
}

