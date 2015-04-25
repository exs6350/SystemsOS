/*
  File:        sfs_bitmap.h
  Description: Bitmap declarations.
  Author:      Justin G., Charles R.
*/

#ifndef SFS_BITMAP_H
#define SFS_BITMAP_H

#include <stdint.h>
#include <limits.h> 

/** Bitmap word size */
typedef uint32_t word_t;

/** Calculations of bits per word, etc */
enum { BITS_PER_WORD = sizeof(word_t) * CHAR_BIT };
#define WORD_OFFSET(b) ((b) / BITS_PER_WORD)
#define BIT_OFFSET(b)  ((b) % BITS_PER_WORD)

/** 
  Set a bit
  Params:  words - pointer to the bitmap
           n     - n'th bit to set
*/
void set_bit(word_t* words, uint32_t n);

/** 
  Clear a bit
  Params:  words - pointer to the bitmap
           n     - n'th bit to clear
*/
void clear_bit(word_t* words, uint32_t n);

/** 
  Get a bit
  Params:  words - pointer to the bitmap
           n     - n'th bit to get
  Returns: n'th bit's value
*/
int get_bit(word_t* words, uint32_t n);

/** 
  Find the first bit with a value of 0
  Params:  words - pointer to the bitmap
  Returns: position of the said bit
*/
uint32_t first_zero_bit(word_t* words);

#endif
