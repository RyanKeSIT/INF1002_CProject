#include "./tools/print.c"
#include "./tools/splice.c"
#include <stdio.h>
#include <string.h>

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
void delete_operation(StudentRecords *s, int recordCount) {
  int targetedStudentID;
  char output;

  printf("P3_4: DELETE ID=");
  // Get the student ID record to delete
  scanf("%d", &targetedStudentID);

  // Get to the row of the student ID to delete
  for (int i = 0; i < recordCount; i++) {
    // Check if student ID is found
    if (s[i].ID == targetedStudentID) {
      char confirmationInput;
      print(
          "CMS: Are you sure you want to delete record with ID=%d? Type \"Y\" "
          "to Confirm or type \"N\" to cancel.\n",
          targetedStudentID);
      print("P3_4: ");
      // Get the confirmation input and force to upper case
      scanf("%s", &confirmationInput);

      if (strcmp(&confirmationInput, "Y") == 0 ||
          strcmp(&confirmationInput, "y") == 0) {
        // Delete the record
        splice(s, &recordCount, i);

        // Print success action
        print("CMS: The record with ID=%d is successfully deleted.",
              targetedStudentID);
      } else if (strcmp(&confirmationInput, "N") == 0 ||
                 strcmp(&confirmationInput, "n") == 0) {
        print("CMS: The deletion is cancelled.\n");
        // Retry this whole function again
        delete_operation(s, recordCount);
      }

      //   Decrement the student count
      recordCount = recordCount - 1;
      return;
    }

    // Check if list is exhausted
    else if (i == recordCount - 1) {
      print("CMS: The record with ID=%d does not exist.\n", targetedStudentID);
      delete_operation(s, recordCount);
    }
  }
}

void save_operation() {}

void sort_operation() {}

void summary_statics_operation() {}

void unique_operation() {}
