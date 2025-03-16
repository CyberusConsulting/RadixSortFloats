#pragma once
/* ---------------------------------------------------------------------------
Copyright:      Cyberus Consulting, (c) 2024
Name:           RadixSortFloats.h
Description:    Radix sort a 32-bit float array by treating its values as unsinged 32-bit integers
Created:        12/06/2024

NOTES:

TODO:
    Todo items are marked with a $$$ comment

Revisions:

--------------------------------------------------------------------------- */

#include <stdint.h>

// STOP: Don't fall for the constexpr hype!
// We need these macros to be injected at compile time.
// Replacing them with constexpr functions will add runtime function overhead.

#define RSF_SHIFT_RIGHT_MASK_8(i,j)		(((i) >> (j)) & 0x000000FF)
#define RSF_IS_NEGATIVE(i)		        ((i) >> 31)
#define RSF_XOR_POSITIVE(i)           ((i) ^ 0x7FFFFFFF)

uint32_t* RadixSortFloatsAsInts(uint32_t elem_count, uint32_t* input, uint32_t* scratchpad, bool ascending);
