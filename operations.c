#include "./tools/splice.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int open_operation() {}

void showall_operation() {}

void insert_operation() {}

void query_operation() {}

void update_operation(StudentRecords *s, int recordCount, char *command) {
  int targetedStudentID;
  char targetedField[20];
  char targetedValue[100];
  //   This is parsing the command e.g. `UPDATE ID=2501234 Mark=69.8`
  sscanf(command, "UPDATE ID=%d %[^=]=%99[^\n]", &targetedStudentID,
         targetedField, targetedValue);

  // Get to the row of the student ID to edit
  for (int i = 0; i < recordCount; i++) {
    // Check if student ID is found
    if (s[i].ID == targetedStudentID) {
      // Update the struct field based on supplied key
      if (_stricmp(targetedField, "Programme") == 0) {
        strncpy(s[i].Programme, targetedValue, sizeof s[i].Programme - 1);
        printf("CMS: The record with ID=%d is successfully updated.",
               targetedStudentID);
      } else if (_stricmp(targetedField, "Mark") == 0) {
        s[i].Mark = atof(targetedValue);
        printf("CMS: The record with ID=%d is successfully updated.",
               targetedStudentID);
      }

      return;
    }

    // Check if list is exhausted
    else if (i == recordCount - 1) {
      printf("CMS: The record with ID=%d does not exist.", targetedStudentID);
    }
  }
}

void delete_operation(StudentRecords *s, int recordCount, char *command) {
  int targetedStudentID;
  // Get the student ID record to delete
  sscanf(command, "DELETE ID=%d", &targetedStudentID);

  // Get to the row of the student ID to delete
  for (int i = 0; i < recordCount; i++) {
    // Check if student ID is found
    if (s[i].ID == targetedStudentID) {
      char confirmationInput;
      printf(
          "CMS: Are you sure you want to delete record with ID=%d? Type \"Y\" "
          "to Confirm or type \"N\" to cancel.\n",
          targetedStudentID);
      printf("P3_4: ");
      // Get the confirmation input and force to upper case
      scanf("%s", &confirmationInput);

      if (_stricmp(&confirmationInput, "Y") == 0) {
        // Delete the record
        splice(s, &recordCount, i);

        // Print success action
        printf("CMS: The record with ID=%d is successfully deleted.",
               targetedStudentID);
      } else if (_stricmp(&confirmationInput, "N") == 0) {
        printf("CMS: The deletion is cancelled.\n");
        // Retry this whole function again
      }

      //   Decrement the student count
      recordCount = recordCount - 1;
      return;
    }

    // Check if list is exhausted
    else if (i == recordCount - 1) {
      printf("CMS: The record with ID=%d does not exist.\n", targetedStudentID);
    }
  }
}

void save_operation() {}

void sort_operation() {}

void summary_statics_operation() {}

void unique_operation() {}
