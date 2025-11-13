#include "cms.h"
#include "print.c"
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
void delete_operation(StudentRecords *s, int *recordCount) {
  int targetedStudentID;
  char output;

  printf("P3_4: DELETE ID=");
  // Get the student ID record to delete
  scanf("%d", &targetedStudentID);

  // Get to the row of the student ID to delete
  for (int i = 0; i < *recordCount; i++) {
    // Check if student ID is found
    if (s[i].ID == targetedStudentID) {
      print(
          "CMS: Are you sure you want to delete record with ID=%d? Type \"Y\" "
          "to Confirm or type \"N\" to cancel.",
          targetedStudentID);

      //   Decrement the student count
      *recordCount = *recordCount - 1;
      return;
    }

    // Check if list is exhausted
    else if (i == *recordCount - 1) {
      print("CMS: The record with ID=%d does not exist.", targetedStudentID);
      return;
    }
  }
}

void save_operation() {}

void sort_operation() {}

void summary_statics_operation() {}

void unique_operation() {}
