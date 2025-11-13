#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

// Function to safely print to standard output.
void print(const char *fmtStr, ...) {
  // Determine buffer size
  va_list args;
  va_start(args, fmtStr);
  int requiredBufferSize = vsnprintf(NULL, 0, fmtStr, args) + 1;
  va_end(args);

  //   Allocate the buffer
  char *result_string = (char *)malloc(requiredBufferSize);
  va_start(args, fmtStr);
  vsnprintf(result_string, requiredBufferSize, fmtStr, args);
  va_end(args);

  //   Print to standard output
  printf("%s", result_string);

  //   Free the malloc memory after use to prevent memleak
  free(result_string);
}