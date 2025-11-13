#include "cms.h"
#include <stdio.h>

int open_operation() {}

void showall_operation() {}

void insert_operation() {}

void query_operation() {}

// Me
void update_operation() {
  // Get the student ID record to edit

  // Get to the row of the student ID to edit

  // Get the column of the row record to edit

  // Return the edited data to main process
}

// Me
void delete_operation(StudentRecords s, int studentCount) {
  int targetedStudentID;
  char output;

  printf("P3_4: DELETE ID=");
  // Get the student ID record to delete
  scanf("%d", &targetedStudentID);

  // Get to the row of the student ID to delete
  for (int i = 0; i < studentCount; i++) {
    // Check if student ID is found
    if (s[i].ID == targetedStudentID) {
      printf("yay");

      //   Decrement the student count
      studentCount--;
    }

    // Check if list is exhausted
    else if (i == studentCount - 1) {
      // Formatting strings
      char *format_string = "CMS: The record with ID=%d does not exist.";
      char result_string[64];
      snprintf(result_string, 64, format_string, targetedStudentID);

      printf("%s", result_string);

      //   Return array of structs as-is
    }
  }

  // Delete the record

  // Return data to main process
}

void save_operation() {}

void sort_operation() {}

void summary_statics_operation() {}

void unique_operation() {}
