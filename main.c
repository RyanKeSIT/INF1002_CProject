#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "cms.h"
#include "operations.c"
#include <stdio.h>

int main(){
    db = records; //point to records array in cms.h
    printf("\nDeclaration\n");
    printf("SIT\'s policy on copying does not allow the students to copy source code as well as assessment solutions from another person AI or other places. It is the student responsibility to guarantee that their assessment solutions are their own work. Meanwhile, the students must also ensure that their work is not accessible by others. Where such plagiarism is detected, both of the assessments involved will receive ZERO mark.\n");
    printf("We hereby declare that:\n");
    printf("• We fully understand and agree to the above mentioned plagiarism policy.\n");
    printf("• We did not copy any code from others or from other places.\n");
    printf("• We did not share our codes with others or upload to any other places for public access and will not do that in the future.\n");
    printf("• We agree that our project will receive Zero mark if there is any plagiarism detected.\n");
    printf("• We agree that we will not disclose any information or material of the group project to others or upload to any other places for public access.\n");
    printf("• We agree that we did not copy any code directly from AI generated sources\n");
    printf("Declared by: P3-4\n");
    printf("Team members:\n");
    printf("1. Lau Ning Xuan\n");
    printf("2. Low Dong Han\n");
    printf("3. Ke Zhijie Ryan\n");
    printf("4. Ang Hung Ling\n");
    printf("5. Muhammad Irfan Bin Md Rosdi\n");
    printf("Date: 25 November 2025 (Tuesday)\n\n");
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
            // Open the database file and read in all the record
            open_operation(filename);
        }
        else if (strcmp(commandPrefix, "SHOW") == 0){
            // Display all the current records in the database
            showall_operation();
        }
        else if (strcmp(commandPrefix, "INSERT" ) == 0){
            // Insert a new data record.
            insert_operation(command);
        }
        else if (strncmp(commandPrefix, "QUERY", 5) == 0){
            // Search if there is any existing record with a given student ID
            query_operation(command);
        }
        else if (strncmp(commandPrefix, "UPDATE", 6) == 0){
            // Update the data for a record with a given student ID
            update_operation();
        }
        else if (strcmp(commandPrefix, "DELETE") == 0){
            //  Delete the record with a given student ID
            delete_operation();
        }
        else if (strcmp(commandPrefix, "SAVE") == 0){
            //Only save when file is opened
            if (!databaseLoaded) {
                printf("CMS: Database is not loaded. Please open the database first.\n");
            } 
            else {
                // Save all the current records into the database file
                save_operation(DB_FILENAME, db, recordCount, custom_column, num_custom_cols);
            }
        }
        else if (strcmp(commandPrefix, "SORT") == 0) {
            // Show summary of Total number of students, Average mark, Highest and lowest mark
            sort_operation();
        }
        else if (strcmp(commandPrefix, "SUMMARY") == 0) {
            // Show summary of Total number of students, Average mark, Highest and lowest mark
            summary_statics_operation();
        }
        else if (strcmp(commandPrefix, "ADD") == 0){
            // Add new columns
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