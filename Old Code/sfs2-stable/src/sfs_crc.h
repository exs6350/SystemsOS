/*
  File:        sfs_crc.h
  Description: CRC32/64 declarations.
  Author:      Charles R.
*/

#ifndef SFS_CRC_H
#define SFS_CRC_H

#include <stdint.h>

/** 
  Compute a crc32 of first half of the message, get the result
  Params:  message - pointer to the queue
           length  - length of the message
  Returns: crc32 of message
*/
uint32_t crc32(uint8_t* message, uint32_t length);

/** 
  Compute a crc32 of first half of the message, shift to low
  Compute a crc32 of second half of the message, put in hi, get the result
  Assert:  message length is divisible by 2
  Params:  message - pointer to the queue
           length  - length of the message
  Returns: crc64 of message
*/
uint64_t crc64(uint8_t* message, uint32_t length);

#endif
