#include "../cms.h"
#include <stdio.h>

// Function to remove an array element at index (zero-based)
void splice(StudentRecords records[], int recordCount, int indexToRemove) {
  // Check for invalid inputs
  if (indexToRemove < 0 || indexToRemove >= recordCount) {
    printf("Invalid index for removal.\n");
    return;
  }

  //   Shift any other array index ahead to replace
  for (int i = indexToRemove; i < recordCount - 1; i++) {
    records[i] = records[i + 1];
  }

  //   Reduce recordCount of array
  recordCount--;
}