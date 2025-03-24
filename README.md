# Radix Sort Floats

### Radix sort a 32-bit float array by treating its values as unsinged 32-bit integers


### Performance:
Without additional compiler optimizations (/O1 /O2 /O3 etc) or using Intrinsics, 
this results in a 2x or better speed improvement (depending on your array size, hardware and environment)
when compared to using the built-in qsort() function.
The larger the input array, the better the performance gains.

### NOTE: 
This function needs an additional empty array of the same size as the float input to be used as a temporary scratchpad. 
This will double your memory footprint!
The function could be refactored to only use the input array and shuffle values in-place but it would loose some of its performance gains.

### How it works:
- Postive floats have a bitpattern where smaller float values have a smaller HEX value.
- Negative floats have a reverse bitpattern where smaller float values have a larger HEX value.
- Negative floats *always* have larger HEX values than postive floats.

### We exploit this pattern to correctly sort floats:
- We XOR postive floats with 0x7FFFFFFF (for comparison only) to reverse the bit pattern to aligned with negative floats.
- This allows us to place them correctly in the sorted array according to their actual float value.
- As a side effect, since IEEE floats can have negative zeros (0x80000000), this also sorts -0.0 correctly.

```
-FLT_MAX   = 0xFF7FFFFF            = 0xFF7FFFFF
-FLT_MIN   = 0x80800000            = 0x80800000
-0.0       = 0x80000000            = 0x80000000
 0.0       = 0x00000000    XOR     = 0x7FFFFFFF
 FLT_MIN   = 0x00800000    XOR     = 0x7F7FFFFF
 FLT_MAX   = 0x7F7FFFFF    XOR     = 0x00800000
```

### TODO:
- Make a version using double/uint64_t
- Make a version without the check for negative numbers, for example when dealing with normalized (0.0 - 1.0) float values
- Make a version that doesn't require a temporary buffer
- Port the function to other languages
- Add an Assembler version of the radix sort function
