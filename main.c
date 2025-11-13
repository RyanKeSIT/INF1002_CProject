#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "cms.h"
#include "operations.c"

#define DB_FILENAME "P3_4-CMS.txt"
StudentRecords *db = NULL;
int recordCount; 
int num_custom_cols = 0;
newColumn custom_column[MAX_CUSTOM_COLUMN_NO];

int main(){
    
    int operation_no; 
    int input;

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
        printf("11. End Program\n");
        printf("Please Select an Operation to Perform: ");
        input = scanf("%d", &operation_no);

        //Checks if input is a decimal, else keeps looping for valid input
        if (input != 1) {
            printf("\nInvalid input. Please enter a number from 1 to 10.\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF); //Clears the input buffer
            operation_no = 0; // Set to 0 so the loop continues
        }

        switch (operation_no){
            case 1:
                open_operation();
                break;
            case 2:
                showall_operation();
                break;
            case 3:
                insert_operation();
                break;
            case 4:
                query_operation();
                break;
            case 5:
                update_operation();
                break;
            case 6:
                delete_operation();
                break;
            case 7:
                save_operation(DB_FILENAME, db, recordCount, custom_column, num_custom_cols);
                break;
            case 8:
                sort_operation();
                break;
            case 9:
                summary_statics_operation();
                break;
            case 10:
                add_column_operation(db, recordCount, custom_column, &num_custom_cols);
                break;
            case 11:
                printf("\nExiting CMS...");
                break;
        } 
    }
    while (operation_no!=11);
    
    return 0;
}