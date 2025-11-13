#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cms.h"

int open_operation() {

}

void showall_operation() { 

}

void insert_operation() {

}

void query_operation() {

}

void update_operation() {

}

void delete_operation() {

}

void save_operation(const char *filename, const StudentRecords *db, int recordCount, newColumn custom_column[], int num_custom_cols) {
    FILE *fptr; //pointer for database file
    fptr = fopen(filename, "w"); //To open and write into the database file
    if (fptr == NULL){
        fprintf(stderr,"\nError opening the file.");
        return;
    }
    fprintf(fptr, "Database Name: CMS\n");
    fprintf(fptr, "Authors: Assistant Prof Oran Zane Devilly\n"); 
    fprintf(fptr, "\n"); 
    fprintf(fptr, "Table Name: StudentRecords\n");
    fprintf(fptr, "ID\tName\t\tProgramme\t\tMark"); 
    // Write custom headers
    for (int j = 0; j < num_custom_cols; j++) {
        fprintf(fptr, "\t%s", custom_column[j].name);
    }
    fprintf(fptr, "\n"); // End of headers

    int recordsWritten = 0; //Number of line written 
    if (recordCount > 0 && db!=NULL){
        //Save contents of the studentRecord struct to file
        for (int i = 0; i < recordCount; i++) {
            // Writing record fields separated by tab (\t)
            fprintf(fptr, "%d\t%s\t%s\t%.1f",db[i].ID, db[i].Name, db[i].Programme, db[i].Mark);
            //Save custom data
            for (int j = 0; j < num_custom_cols; j++) {
                fprintf(fptr, "\t%s", db[i].custom_column[j]);
            }
            fprintf(fptr, "\n"); // End of line
            recordsWritten++;
         }
    }

    // Check if saving was successful
    if (recordsWritten == recordCount) {
        printf("The database file “%s” is successfully saved\n", filename);
    }
    else {
        printf("Error Saving to File! Only wrote %d out of %d records.", recordsWritten, recordCount);
    }
    fclose(fptr);
}

void sort_operation(){

}

void summary_statics_operation() {

}

void add_column_operation(StudentRecords *db, int recordCount, newColumn custom_column[], int *num_custom_cols) {
    printf("\n---Add Column ---\n");
    if (*num_custom_cols >= MAX_CUSTOM_COLUMN_NO) {
        printf("Maximum number of custom columns (%d) reached.\n", MAX_CUSTOM_COLUMN_NO);
        return;
    }
    char response[10];
    //Loop to ensure valid column name and column data before adding them 
    do{
        int col_index = *num_custom_cols;
        printf("Enter new column name: ");
        int c;
        //Get new column name
        while ((c = getchar()) != '\n' && c != EOF);
        if (scanf("%20s", custom_column[col_index].name) != 1) {
            printf("Invalid input.\n");
            return;
        }
        //Get column type
        int type_validity = 0; 
        printf("Enter the type of the new column: ");
        //Validate column type
        while (!type_validity) {

            if (scanf("%20s", custom_column[col_index].type) != 1) {
                printf("Input error. Operation aborted.\n");
                while ((c = getchar()) != '\n' && c != EOF); // Clear buffer
                return;
            }
            // Normalize input to lowercase for case-insensitive checking
            for (int i = 0; custom_column[col_index].type[i]; i++){
              custom_column[col_index].type[i] = tolower(custom_column[col_index].type[i]);
            }

            // Check against allowed types
            if (strcmp(custom_column[col_index].type, "string") == 0 || strcmp(custom_column[col_index].type, "float") == 0 ||strcmp(custom_column[col_index].type, "int") == 0) {
                type_validity = 1; // Column type valid
            } 
            else {
                // Prompt for type input if invalid type
                printf("Invalid type '%s'. Please enter a valid type (e.g. string, float, or int): ", custom_column[col_index].type);
                while ((c = getchar()) != '\n' && c != EOF); // Clear buffer for next input
            }
        }

        //Get data for existing records for new column
        if (recordCount > 0 && db != NULL) {
            printf("\n--- Entering Data for %s ---\n", custom_column[col_index].name);
            for (int i = 0; i < recordCount; i++) {
                printf("Enter %s for %s (ID: %d): ", custom_column[col_index].name, db[i].Name, db[i].ID);
                if (scanf("%20s", db[i].custom_column[col_index]) != 1) {
                    printf("Invalid input.\n");
                    while ((c = getchar()) != '\n' && c != EOF); 
                    strcpy(db[i].custom_column[col_index], "ERROR");
                }
            }
        } 
        else {
            printf("\nDatabase is empty. Column defined, but no data to input yet.\n");
        }
        (*num_custom_cols)++;
        printf("Successfully added column '%s'.\n", custom_column[col_index].name);
        
        // Ask if the user wants to add another column
        if (*num_custom_cols < MAX_CUSTOM_COLUMN_NO) {
            printf("Add another column? (yes/no): ");
            scanf("%10s", response);
        } else {
            printf("Maximum number of custom columns (%d) reached.\n", MAX_CUSTOM_COLUMN_NO);
            strcpy(response, "no");
        }
    }
    while( *num_custom_cols < MAX_CUSTOM_COLUMN_NO && (strcmp(response, "yes") == 0 || strcmp(response, "y") == 0));
}

