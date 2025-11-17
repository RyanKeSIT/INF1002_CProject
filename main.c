#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "cms.h"
#include "operations.c"

int main(){
    db = records; //point to records array in cms.h

    //Loop main menu until user input "EXIT"
    do {    
        printf("==================================\n");
        printf("Class Management System (CMS) Menu\n");
        printf("==================================\n");
        printf("  OPEN                              - Open the database file & read in all the records\n");
        printf("  SHOW ALL                          - Show all records\n");
        printf("  INSERT ID=<id>                    - Insert a new record\n");
        printf("  QUERY ID=<id>                     - Query a record by ID\n");
        printf("  UPDATE ID=<id> <field>=<value>    - Update a record by ID\n");
        printf("  DELETE ID=<id>                    - Delete a record by ID\n");
        printf("  SAVE                              - Save the database file\n");
        printf("  SORT                              - Sort the records\n");
        printf("  UNIQUE                            - Unique Feature\n");
        printf("  EXIT                              - Exit the program\n");
        printf("Please Select an Operation to Perform: ");
        printf("\nP3_4: ");
        scanf(" %[^\n]", command); // Read user input until newline

        // Extract the first word from the command for case-insensitive comparison
        char commandPrefix[20];
        sscanf(command, "%19s", commandPrefix); // Store the first word of the command in commandPrefix
        toUpperCase(commandPrefix); // Convert commandPrefix to uppercase

        // Verify the command entered by the user
        if (strcmp(commandPrefix, "OPEN") == 0){
            // Open the database file
            open_operation(filename);
        }
        else if (strcmp(commandPrefix, "SHOW") == 0){
            // Show all records in the database
            showall_operation();
        }
        else if (strcmp(commandPrefix, "INSERT" ) == 0){
            // Call the insert function
            insert_operation(command);
        }
        else if (strncmp(commandPrefix, "QUERY", 5) == 0){
            query_operation();
        }
        else if (strncmp(commandPrefix, "UPDATE", 6) == 0){
            update_operation();
        }
        else if (strcmp(commandPrefix, "DELETE") == 0){
            delete_operation();
        }
        else if (strcmp(commandPrefix, "SAVE") == 0){
            //Only save when file is opened
            if (!databaseLoaded) {
                printf("CMS: Database is not loaded. Please open the database first.\n");
            } 
            else {
                save_operation(DB_FILENAME, db, recordCount, custom_column, num_custom_cols);
            }
        }
        else if (strcmp(commandPrefix, "ADD") == 0){
            add_column_operation(command, custom_column, &num_custom_cols);
        }
        else if (strcmp(commandPrefix, "EXIT") == 0){
            printf("Exiting the program.\n");
            break;
        }
        else {
            // If the command is invalid, prompt the user to try again
            printf("Invalid command. Please try again.\n");
        }
    }
    while (1);
    
    return 0;
}