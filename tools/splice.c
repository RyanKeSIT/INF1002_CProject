#include "../cms.h"
#include <stdio.h>

// Function to remove an array element at index (zero-based)
void splice(StudentRecords arr[], int *size, int indexToRemove) {
  // Check for invalid inputs
  if (indexToRemove < 0 || indexToRemove >= *size) {
    printf("Invalid index for removal.\n");
    return;
  }

  //   Shift any other array index ahead to replace
  for (int i = indexToRemove; i < *size - 1; i++) {
    arr[i] = arr[i + 1];
  }

  //   Reduce size of array
  (*size)--;
}