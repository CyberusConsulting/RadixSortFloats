/* ---------------------------------------------------------------------------
Copyright:      Cyberus Consulting, (c) 2024
Name:           TestRadixSort.h
Description:    Test the radix sort function
Created:        12/06/2024

NOTES:

TODO:
    Todo items are marked with a $$$ comment

Revisions:

--------------------------------------------------------------------------- */

#include <windows.h>
#include <iostream>
#include <cstdlib> // for rand() and srand()
#include <ctime>   // for time()

#include "RadixSortFloats.h"


constexpr bool SORT_ASCENDING     = true;
constexpr bool SORT_DESCENDING    = false;
constexpr bool SHOW_DEBUG         = true;
constexpr bool HIDE_DEBUG         = false;
constexpr int USE_RADIX_SORT      = 0;
constexpr int USE_QSORT           = 1;

LARGE_INTEGER frequency;
LARGE_INTEGER start, end;



//---------------------------------------------------------
// Test the performance of both the built-in qsort()
// and our optimized radix sort functions
//---------------------------------------------------------
static void TestFloatSort(int sort_type, int num_cycles, int num_values, bool ascending, bool show_debug) {

  printf(
    "\nTestFloatSort():\nsort_type: %d, num_cycles: %d, num_values: %d, ascending: %d, show_debug: %d\n\n",
    sort_type, num_cycles, num_values, ascending, show_debug
  );

  // qsort float compare functions
  auto compare_floats_asc = [](const void* a, const void* b) {
    float fa = *(const float*)a;
    float fb = *(const float*)b;
    return (fa > fb) - (fa < fb);
    };

  auto compare_floats_dsc = [](const void* a, const void* b) {
    float fa = *(const float*)a;
    float fb = *(const float*)b;
    return (fa < fb) - (fa > fb);
    };

  // Float array(s)
  float* float_array = new float[num_values] {};
  float* float_array_temp = new float[num_values] {};

  // Fill the test array with random numbers
  // We shift the numbers by f_max/2 to generate some negative numbers as well
  float f_min = 0.0;
  float f_max = 20000.0;
  for (int i=0; i<num_values; i++) {
    float_array[i] = (f_min + (float)rand() / (float)RAND_MAX * (f_max - f_min)) - (f_max / 2.0f);
  }

  // If we have enough space, we add some edgecase test values
  if (num_values > 21) {
    float_array[10] =  0.0f;
    float_array[11] =  1.0f;
    float_array[12] = -1.0f;
    float_array[13] = -0.0f;
    float_array[14] =  0.00002f;
    float_array[15] = -0.00002f;
    float_array[16] =  0.99998f;
    float_array[17] = -0.99998f;
    float_array[18] =  FLT_MAX;
    float_array[19] =  FLT_MIN;
    float_array[20] = -FLT_MAX;
    float_array[21] = -FLT_MIN;
  }

  if (show_debug) {
    printf("Unsorted array:\n");
    for (int i=0; i<num_values; i++) {
      printf("%f ", float_array[i]);
    }
    printf("\n\n");
  }

  uint32_t* float_int_ptr = (uint32_t*)&float_array[0];
  uint32_t* float_int_ptr_temp = (uint32_t*)&float_array_temp[0];
  uint32_t* int_ptr_sorted = float_int_ptr;
  float* float_ptr_sorted = float_array;

  // Sort the float array using qsort?
  if (USE_QSORT == sort_type) {
    if (ascending) {

      QueryPerformanceCounter(&start);    // Start the timer
      for (int c=0; c<num_cycles; c++) {  // Repeat num_cycles times
        qsort(float_array, num_values, sizeof(float), compare_floats_asc);
      }
      QueryPerformanceCounter(&end);      // Stop the timer

    } else {

      QueryPerformanceCounter(&start);    // Start the timer
      for (int c=0; c<num_cycles; c++) {  // Repeat num_cycles times
        qsort(float_array, num_values, sizeof(float), compare_floats_dsc);
      }
      QueryPerformanceCounter(&end);      // Stop the timer
    }

  // Sort the array using Radix Sort?
  } else {

    QueryPerformanceCounter(&start);    // Start the timer
    for (int c=0; c<num_cycles; c++) {  // Repeat num_cycles times
      int_ptr_sorted = RadixSortFloatsAsInts((uint32_t)num_values, float_int_ptr, float_int_ptr_temp, ascending);
    }
    QueryPerformanceCounter(&end);      // Stop the timer

    float_ptr_sorted = (float*)int_ptr_sorted;
  }

  // Calculate elapsed time in seconds
  double elapsed = (end.QuadPart - start.QuadPart) / (double)frequency.QuadPart;
  printf("Elapsed time: %.6f seconds\n\n", elapsed);

  // Print the results
  if (show_debug) {
    printf("Sorted array:\n");
    for (int i=0; i<num_values; i++) {
      printf("%f ", float_ptr_sorted[i]);
    }
    printf("\n\n");

    printf("Sorted array in HEX:\n");
    for (int i=0; i<num_values; i++) {
      printf("%08X ", int_ptr_sorted[i]);
    }
    printf("\n\n");

    printf("Sorted array in HEX with -XOR:\n");
    for (int i=0; i<num_values; i++) {
      uint32_t* pval = &int_ptr_sorted[i];
      if (RSF_IS_NEGATIVE(*pval)) {
        printf("%08X ", int_ptr_sorted[i]);
      } else {
        printf("%08X ", (RSF_XOR_POSITIVE(int_ptr_sorted[i])));
      }
    }
    printf("\n\n");
  }
}

//---------------------------------------------------------
int main() {

  // Get the frequency of the performance counter
  QueryPerformanceFrequency(&frequency);

  // Seed with fixed value for repeatable results
  srand(123456789);

  //---------------------------------------------------------
  // Run the test(s)
  //---------------------------------------------------------

  //TestFloatSort(USE_QSORT,      1, 100, SORT_ASCENDING, SHOW_DEBUG);
  //TestFloatSort(USE_RADIX_SORT, 1, 100, SORT_ASCENDING, SHOW_DEBUG);

  TestFloatSort(USE_QSORT,      1000, 10000, SORT_ASCENDING, HIDE_DEBUG);
  TestFloatSort(USE_RADIX_SORT, 1000, 10000, SORT_ASCENDING, HIDE_DEBUG);

  return 0;
}
