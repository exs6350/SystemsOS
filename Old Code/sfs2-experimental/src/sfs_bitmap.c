/*
  File:        sfs_bitmap.c
  Description: Bitmap definitions.
  Author:      Justin G., Charles R.
*/

#include "sfs_bitmap.h"

/******************************************************************************/
/*                         Public Bitmap Operations                           */
/*                   All documentation is found in header                     */
/******************************************************************************/

void set_bit(word_t* words, uint32_t n) { 
  words[WORD_OFFSET(n)] |= (1 << BIT_OFFSET(n));
}

void clear_bit(word_t* words, uint32_t n) {
  words[WORD_OFFSET(n)] &= ~(1 << BIT_OFFSET(n));
}

int get_bit(word_t* words, uint32_t n) {
  word_t bit = words[WORD_OFFSET(n)] & (1 << BIT_OFFSET(n));
  return bit != 0; 
}

uint32_t first_zero_bit(word_t* words) {
  uint32_t offset = -1;
  //find a word with an empty bit
  while(words[++offset] == UINT32_MAX);

  //find the bit
  for(int i = 0; i < 32; i++ ){
    if(get_bit(&words[offset], i) == 0) {
      return (offset * 32) + i;
    }
  }
  return -1;
}
