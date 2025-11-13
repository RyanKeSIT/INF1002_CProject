/*
https://share.google/jq82YvI5CNW7142SF
https://share.google/s5Psd3pMrIGDPOtcX
*/

#include "operations.c"
#include <stdio.h>

#define DB_FILENAME "P3_4-CMS.txt"

int main() {
  int operation_no;
  int input;
  StudentRecords s = {{2306969, "Hello", "INF1001", 99.9},
                      {2306970, "Hello 1", "INF1002", 88.8},
                      {2306971, "Hello 3", "INF1003", 49.9}};
  int studentCount = sizeof s / sizeof s[0];

  do {
    printf("==================================\n");
    printf("Class Management System (CMS) Menu\n");
    printf("==================================\n");
    printf("1. Open\n");
    printf("2. Show All\n");
    printf("3. Insert\n");
    printf("4. Query\n");
    printf("5. Update\n");
    printf("6. Delete\n");
    printf("7. Save\n");
    printf("8. Sort Student Records\n");
    printf("9. Show summary\n");
    printf("10. Unique Feature\n");
    printf("Please Select an Operation to Perform: ");
    input = scanf("%d", &operation_no);

    // Checks if input is a decimal, else keeps looping for valid input
    if (input != 1) {
      printf("\nInvalid input. Please enter a number from 1 to 10.\n");
      int c;
      while ((c = getchar()) != '\n' && c != EOF)
        ;               // Clears the input buffer
      operation_no = 0; // Set to 0 so the loop continues
    }

    switch (operation_no) {
    case 1:
      open_operation();
    case 2:
      showall_operation();
    case 3:
      insert_operation();
    case 4:
      query_operation();
    case 5:
      update_operation();
    case 6:
      delete_operation(s, studentCount);
    case 7:
      save_operation();
    case 8:
      sort_operation();
    case 9:
      summary_statics_operation();
    case 10:
      unique_operation();
    }
  } while (input != 1 || operation_no < 1 || operation_no > 10);

  return 0;
}