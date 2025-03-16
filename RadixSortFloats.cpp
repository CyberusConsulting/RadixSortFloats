/* ---------------------------------------------------------------------------
Copyright:      Cyberus Consulting, (c) 2024
Name:           RadixSortFloats.cpp
Description:    Radix sort a 32-bit float array by treating its values as unsinged 32-bit integers
Created:        12/06/2024

NOTES:

TODO:
    Todo items are marked with a $$$ comment

Revisions:

--------------------------------------------------------------------------- */

#include "RadixSortFloats.h"


//---------------------------------------------------------
// Radix sort a 32-bit float array by treating its values as unsinged 32-bit integers
// 
// Performance:
// Without additional compiler optimizations (/O1 /O2 /O3 etc) or using Intrinsics, 
// this results in a 2x or better speed improvement (depending on your array size, hardware and environment)
// when compared to using the built-in qsort() function.
// The larger the input array, the better the performance gains.
// 
// NOTE: 
// This function needs an additional empty array of the same size as the float input to be used as a temporary scratchpad. 
// This will double your memory footprint!
// The function could be refactored to only use the input array and shuffle values in-place but it would loose some of its performance gains.
// 
// How it works:
// - Postive floats have a bitpattern where the smaller the float value, the smaller the HEX value.
// - Negative floats have a reverse bitpattern where the smaller the float value, the larger the HEX value.
// - Negative floats *always* have larger HEX values than postive floats.
// 
// We exploit this pattern to correctly sort floats:
// - We XOR postive floats with 0x7FFFFFFF (for comparison only) to reverse the bit pattern to aligned with negative floats.
// - This allows us to place them correctly in the sorted array according to their actual float value.
// - As a side effect, since IEEE floats can have negative zeros (0x80000000), this also sorts -0.0 correctly.
// 
// -FLT_MAX   = 0xFF7FFFFF            = 0xFF7FFFFF
// -FLT_MIN   = 0x80800000            = 0x80800000
// -0.0       = 0x80000000            = 0x80000000
//  0.0       = 0x00000000    XOR     = 0x7FFFFFFF
//  FLT_MIN   = 0x00800000    XOR     = 0x7F7FFFFF
//  FLT_MAX   = 0x7F7FFFFF    XOR     = 0x00800000
// 
// TODO:
// - Make a version using double/uint64_t
// - Make a version without the check for negative numbers, for example when dealing with normalized (0.0 - 1.0) float values
// - Make a version that doesn't require a temporary buffer
// - Port the function to other languages
// 
//---------------------------------------------------------
uint32_t* RadixSortFloatsAsInts(uint32_t element_count, uint32_t* input_buffer, uint32_t* scratchpad, bool ascending) {
  const uint32_t byte_count = 4;
  uint32_t m_index[byte_count][256] ={ {0},{0} };
 
  // Sanity check
  if ((element_count < 2) || (NULL == input_buffer) || (NULL == scratchpad)) { return input_buffer; }

  // Generate a histogram of the byte values for each element
  for (uint32_t e=0; e<element_count; e++) {
    uint32_t val = input_buffer[e];
    if (!IS_NEGATIVE(val)) { val = XOR_POSITIVE(val); }
    m_index[0][(val & 0x000000FF)]++; val >>= 8;
    m_index[1][(val & 0x000000FF)]++; val >>= 8;
    m_index[2][(val & 0x000000FF)]++; val >>= 8;
    m_index[3][(val & 0x000000FF)]++;
  }

  // Convert the histogram counts to indices
  if (ascending) {
    for (uint32_t b=0; b<byte_count; b++) {
      uint32_t pos = 0;
      for (int32_t c=255; c>=0; c--) {
        uint32_t val = m_index[b][c];
        m_index[b][c] = pos;
        pos += val;
      }
    }
  } else {
    for (uint32_t b=0; b<byte_count; b++) {
      uint32_t pos = 0;
      for (uint32_t c=0; c<256; c++) {
        uint32_t val = m_index[b][c];
        m_index[b][c] = pos;
        pos += val;
      }
    }
  }

  // Radix sort the array
  uint32_t* psrc = input_buffer;
  uint32_t* pdst = scratchpad;
  uint32_t* ptmp = NULL;
  uint32_t shift_bits = 0;

  for (uint32_t b=0; b<byte_count; b++) {
    for (uint32_t e=0; e<element_count; e++) {
      uint32_t val = psrc[e];
      if (!IS_NEGATIVE(val)) {
        pdst[m_index[b][SHIFT_RIGHT_MASK_8(XOR_POSITIVE(val), shift_bits)]++] = val;
      } else {
        pdst[m_index[b][SHIFT_RIGHT_MASK_8(val, shift_bits)]++] = val;
      }
    }
    shift_bits += 8;

    // Swap pointers
    ptmp = psrc; psrc = pdst; pdst = ptmp;
  }

  return psrc;
}

