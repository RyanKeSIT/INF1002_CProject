#include "./tools/splice.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*-------------------------------------------------
Convert user input for operation choice to uppercase
----------------------------------------------------*/
void toUpperCase(char *str)
{
    // Loop through each character in the string until reaching the null terminator
    for (int i = 0; str[i]; i++)
    {
        // Convert the current character to uppercase using toupper function
        str[i] = toupper(str[i]);
    }
}

/*-------------------------------------------------
Open the database file and read in all the records
--------------------------------------------------*/
void open_operation(const char *filename)
{
    // Open the file in read mode
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        // If cannot be found or opened, then display error message
        printf("CMS: Cannot locate file, check file name \"%s\".\n", filename);
        return;
    }

    char line[200]; //  to store 200 chars for each line read from the file (we dont expect each line to be more than 200 chars including \0)
    recordCount = 0; // Reset the record count
    int isEmpty = 1; // Flag to check if the file has non-whitespace content

    // Verify if the file is empty and show error
    while (fgets(line, sizeof(line), file))
    {
        // Verify if the line has any non-whitespace character
        for (int i = 0; line[i] != '\0'; i++)
        {
            if (!isspace(line[i]))
            {
                isEmpty = 0;  // Found non-whitespace content and Set flag to indicate the file is not empty
                break;
            }
        }
        if (!isEmpty) break;  // Exit the loop as soon as we find non-empty content
    }

    if (isEmpty) // If the file is empty
    {
        // Display error message when the file has no content and close the file
        printf("CMS: Your file does not have any contents.\n");
        fclose(file);
        return;
    }

    // Reset file pointer to beginning after checking file is empty which reacches EOF
    rewind(file);

    // Load column definitions
    load_column_metadata(filename, custom_column, &num_custom_cols);

    // Initialize tableName to "Unknown" in case we don't find it (error handling)
    strcpy(tableName, "Unknown <There are no table name>");

    // Read each line until we find the table name
    while (fgets(line, sizeof(line), file))
    {
        // Verify if this line contains "Table Name:"
        if (strstr(line, "Table Name:") != NULL)
        {
            // Move pointer to the text after "Table Name:"
            char *pos = strstr(line, "Table Name:") + strlen("Table Name:");

            // Trim any leading whitespace
            while (isspace(*pos)) pos++;

            // Copy the table name to the variable
            strncpy(tableName, pos, sizeof(tableName) - 1);
            tableName[strcspn(tableName, "\n")] = '\0';  // Remove trailing newline for readability

            break;  // Exit loop after finding the table name
        }
    }

    // Add the header as the first "record" in the array
    records[0].ID = 0;  // Set ID to 0 or any special value for the header
    strncpy(records[0].Name, "Name", sizeof(records[0].Name) - 1); // Set "Name" header
    strncpy(records[0].Programme, "Programme", sizeof(records[0].Programme) - 1); // Set "Programme" header
    records[0].Mark = 0.0f;  // Set "Mark" header

    recordCount = 1;  // Start counting number of student records

    // Process each data line
    while (fgets(line, sizeof(line), file))
    {
        // Ignore empty lines or header lines in the file
        if (strstr(line, "ID"))
        {
            continue;
        }

        int id; // Variable to store the ID
        char name[50]; // Variable to store the name
        char programme[50]; // Variable to store the programme name
        float mark; // Variable to store the mark

        
        int matched = sscanf(line, "%d %49[^\t] %49[^\t] %f", &id, name, programme, &mark);

        if (matched == 4) 
        {
           
            records[recordCount].ID = id;
            strncpy(records[recordCount].Name, name, sizeof(records[recordCount].Name) - 1);
            strncpy(records[recordCount].Programme, programme, sizeof(records[recordCount].Programme) - 1);
            records[recordCount].Mark = mark;
            
            // Skip 4 default columns to reach the start of custom columns
            char *current_pos = line; // Pointer to current position in the current line being processed
            
            // Loop through data of default columns to get to custom columns
            for (int i = 0; i < 4; i++) {
                current_pos = strchr(current_pos, '\t'); //Find next tab
                if (current_pos) current_pos++; // Skip the tab to go to next custom column data
            }
            
            // Loop through custom columns to read custom column data
            for (int j = 0; j < num_custom_cols && current_pos; j++) {
                // Find the next tab or newline
                char *next_tab = strchr(current_pos, '\t');
                char *newline = strchr(current_pos, '\n'); 
                
                // Determine the end of this field
                char *field_end = next_tab; // points to tab after the current custom data
                //If there is no more tabs; end of line 
                if (!field_end || (newline && newline < field_end)) {
                    field_end = newline;
                }
                //If tab
                if (field_end) {
                    int field_length = field_end - current_pos; 
                    // Checks if custom data is empty
                    if (field_length > 0) {
                        char field_data[MAX_COLUMN_DATA_LENGTH];
                        strncpy(field_data, current_pos, field_length); //Copy custom data 
                        field_data[field_length] = '\0';
                        
                        // Trim whitespace
                        char *start = field_data; //points to space before field
                        char *end = field_data + strlen(field_data) - 1; //points to space after field
                        while (isspace(*start)) 
                            start++; // Skip spaces
                        while (end > start && isspace(*end)) end--; // Skip trailing spaces
                        *(end + 1) = '\0'; //// Terminate the string at the new end position after trimming
                        
                        // Store data based on column type
                        if (strcmp(custom_column[j].type, "int") == 0) {
                            records[recordCount].custom_column[j].int_value = atoi(start);
                        } 
                        else if (strcmp(custom_column[j].type, "float") == 0) {
                            records[recordCount].custom_column[j].float_value = atof(start);
                        } 
                        else if (strcmp(custom_column[j].type, "string") == 0) {
                            strncpy(records[recordCount].custom_column[j].string_value, start, sizeof(records[recordCount].custom_column[j].string_value) - 1);
                            records[recordCount].custom_column[j].string_value[sizeof(records[recordCount].custom_column[j].string_value) - 1] = '\0';
                        }
                    }
                    current_pos = next_tab ? next_tab + 1 : NULL; //Move to the next custom column data
                }
            }

            recordCount++;
        }
        else 
        {
            
            printf("CMS: Warning: Could not read line: %s\n", line);
        }

        if (recordCount >= MAX_RECORDS) 
        {
            // Display error message and break out of the loop
            printf("CMS: Warning: Maximum record limit reached.\n");
            break;
        }
    }
    fclose(file); 
    databaseLoaded = 1;  
    printf("CMS: The database file \"%s\" is successfully opened.\n", filename); 
    
}

//Load column definitions from separate meta file
void load_column_metadata(const char *filename, newColumn custom_column[], int *num_custom_cols) {
    FILE *fptr = fopen(META_FILENAME, "r");
    if (fptr == NULL) {
        *num_custom_cols = 0; // No meta file exists yet
        return;
    }
    
    // Read number of custom columns
    if (fscanf(fptr, "%d", num_custom_cols) != 1) {
        printf("CMS: Warning: Could not read meta file format.\n");
        fclose(fptr);
        *num_custom_cols = 0;
        return;
    }
    
    // Read each column definition
    for (int i = 0; i < *num_custom_cols; i++) {
        if (fscanf(fptr, "%s %s %d", custom_column[i].name, custom_column[i].type, &custom_column[i].length) != 3) {
            printf("CMS: Warning: Error reading custom columns %d from meta file.\n", i);
            break;
        }
    }
    
    fclose(fptr);
}

/*----------------------------------------------------------------
Function to To display all the current records in the read-in data
-----------------------------------------------------------------*/
void showall_operation()
{
    if (!databaseLoaded)
    {
        printf("CMS: Database is not loaded. Please load the database first.\n");
        return;
    }
    if (recordCount <= 0)
    {
        printf("CMS: No records available to display.\n");
        return;
    }
    // Print a message indicating the records will be displayed
    printf("CMS: Here are all the records found in the table \"%s\".\n", tableName);

    // Initialize variables to determine maximum column widths
    int maxNameLength = strlen("Name"); // Start with the length of the "Name" header
    int maxProgrammeLength = strlen("Programme"); // Start with the length of the "Programme" header

    // Loop through each record to find the longest name and programme lengths
    for (int i = 0; i < recordCount; i++)
    {
        int nameLength = (int)strlen(records[i].Name);// Get the length of the current record's name
        int programmeLength = (int)strlen(records[i].Programme); // Get the length of the current record's programme

        if (nameLength > maxNameLength) 
            maxNameLength = nameLength; // Update maxNameLength if the current name is longer
        if (programmeLength > maxProgrammeLength)
            maxProgrammeLength = programmeLength; // Update maxProgrammeLength if the current programme is longer
    }

    maxNameLength += 3;
    maxProgrammeLength += 3;

    // Create format strings dynamically based on the maximum lengths
    char formatHeader[100], formatRow[100];
    // Find max widths for each custom column
    int customWidth[MAX_CUSTOM_COLUMN_NO];

    for (int c = 0; c < num_custom_cols; c++)
    {
        int maxLen = (int)strlen(custom_column[c].name);

        for (int i = 1; i < recordCount; i++)   // start from 1; 0 is header
        {
            char temp[MAX_COLUMN_DATA_LENGTH];

            if (strcmp(custom_column[c].type, "int") == 0)
            {
                snprintf(temp, sizeof(temp), "%d", records[i].custom_column[c].int_value);
            }
            else if (strcmp(custom_column[c].type, "float") == 0)
            {
                snprintf(temp, sizeof(temp), "%.2f", records[i].custom_column[c].float_value);
            }
            else if (strcmp(custom_column[c].type, "string") == 0)
            {
                snprintf(temp, sizeof(temp), "%s", records[i].custom_column[c].string_value);
            }
            else
            {
                temp[0] = '\0';
            }

            int len = (int)strlen(temp);
            if (len > maxLen) 
            maxLen = len;
        }

        customWidth[c] = maxLen + 3;
    }
    // Format for header: %-8s for "ID", dynamic width for "Name" and "Programme", and default for "Mark"
    snprintf(formatHeader, sizeof(formatHeader), "%%-8s %%-%ds %%-%ds %%s\n", maxNameLength+5, maxProgrammeLength+5);
    // Format for rows: %-8d for ID, dynamic width for "Name" and "Programme", and %.1f for "Mark" (1 decimal place)
    snprintf(formatRow, sizeof(formatRow), "%%-8d %%-%ds %%-%ds %%.1f\n", maxNameLength+5, maxProgrammeLength+5);
    
    // Print records: header row (ID == 0) and then data rows
    for (int i = 0; i < recordCount; i++)
    {
        if (records[i].ID == 0)
        {
            // No new columns added
            if (num_custom_cols == 0)
            {
                printf(formatHeader, "ID", records[i].Name, records[i].Programme, "Mark");
            }
            else
            {
                printf("%-8s %-*s %-*s %s", "ID", maxNameLength, records[i].Name, maxProgrammeLength, records[i].Programme, "Mark");

                // Custom column headers
                for (int c = 0; c < num_custom_cols; c++)
                {
                    printf(" %-*s", customWidth[c], custom_column[c].name);
                }
                printf("\n");
            }
        }
        else
        {
            //Print data row
            printf("%-8d %-*s %-*s %.1f", records[i].ID, maxNameLength, records[i].Name, maxProgrammeLength, 
                records[i].Programme, records[i].Mark);

            // Print added Custom column values
            for (int c = 0; c < num_custom_cols; c++)
            {
                if (strcmp(custom_column[c].type, "int") == 0)
                {
                    printf(" %-*d", customWidth[c], records[i].custom_column[c].int_value);    
                }
                else if (strcmp(custom_column[c].type, "float") == 0)
                {
                    printf(" %-*.2f", customWidth[c], records[i].custom_column[c].float_value);
                }
                else if (strcmp(custom_column[c].type, "string") == 0)
                {
                    printf(" %-*s", customWidth[c], records[i].custom_column[c].string_value);
                }
            }
            printf("\n");
        }
    }
}



/*--------------------------
To insert a new data record
---------------------------*/
void insert_operation(const char* command)
{
    // Verify if the database is loaded
    if (!databaseLoaded)
    {
        printf("CMS: Database is not loaded. Please load the database first.\n");
        return;
    }

    int id;  // Variable to store the ID
    char name[50] = {0};  // Array to store the name
    char programme[50] = {0};  // Array to store the programme
    float mark;  // Variable to store the mark

    // Read ID from the command
    if (sscanf(command, "INSERT ID=%d", &id) != 1)
    {
        // If the command does not contain a valid ID, display an error
        printf("CMS: The record insertion contains an invalid command. Please try again.\n");
        return;
    }

    // Verify ID length (convert to string to verify length)
    char idStr[20];
    sprintf(idStr, "%d", id);  // Convert the ID to a string
    int idLen = strlen(idStr); // Get the length of the string
    if (idLen != 7)
    {
        // If the ID is not exactly 7 digits, display an error
        printf("CMS: Student ID must be exactly 7 digits.\n");
        return;
    }

    // Verify if the ID already exists in the records
    if (checkRecordIDExist_operation(id) != -1)
    {
        // If the ID is found, display a duplicate ID error and exit
        printf("CMS: The record with ID=%d already exists.\n", id);
        return;
    }

    // Verify if the command does not contain additional fields (Name, Programme, Mark)
    if (!strstr(command, "Name=") || !strstr(command, "Programme=") || !strstr(command, "Mark="))
    {
        // If additional fields are missing, prompt the user to enter them
        printf("CMS: The record with ID=%d is available. Please enter the details in the format:\n", id);
        printf("Name=<name> Programme=<programme> Mark=<mark>\n");
        return;
    }

    // Read the remaining fields (Name, Programme, Mark)
    char *namePos = strstr(command, "Name=");       // Find the position of "Name="
    char *programmePos = strstr(command, "Programme="); // Find the position of "Programme="
    char *markPos = strstr(command, "Mark=");       // Find the position of "Mark="

    // If any field is missing or invalid, display an error
    if (!namePos || !programmePos || !markPos)
    {
        printf("CMS: The record insertion contains an invalid command. Please try again.\n");
        return;
    }

    // Extract the name field (substring between "Name=" and "Programme=")
    namePos += 5; // Skip "Name="
    int nameLen = programmePos - namePos; // Calculate the length of the name substring
    strncpy(name, namePos, nameLen - 1); // Copy the substring to the 'name' variable
    name[nameLen - 1] = '\0'; // Null-terminate the string

    // Extract the programme field (substring between "Programme=" and "Mark=")
    programmePos += 10; // Skip "Programme="
    int programmeLen = markPos - programmePos; // Calculate the length of the programme substring
    strncpy(programme, programmePos, programmeLen - 1); // Copy the substring to the 'programme' variable
    programme[programmeLen - 1] = '\0'; // Null-terminate the string

    // Extract the mark field (after "Mark=")
    if (sscanf(markPos, "Mark=%f", &mark) != 1)
    {
        // If the mark field is invalid, display an error
        printf("CMS: The record insertion contains an invalid command. Please try again.\n");
        return;
    }

    // Validate the name (must contain only letters and spaces)
    for (int i = 0; name[i]; i++)
    {
        if (!isalpha(name[i]) && !isspace(name[i]))
        {
            // If the name contains invalid characters, display an error
            printf("CMS: The record insertion contains an invalid command. Please try again.\n");
            return;
        }
    }

    // Validate the programme (must contain only letters and spaces)
    for (int i = 0; programme[i]; i++)
    {
        if (!isalpha(programme[i]) && !isspace(programme[i]))
        {
            // If the programme contains invalid characters, display an error
            printf("CMS: The record insertion contains an invalid command. Please try again.\n");
            return;
        }
    }

    // Validate the mark (must be between 0 and 100)
    if (mark < 0 || mark > 100)
    {
        // If the mark is outside the valid range, display an error
        printf("CMS: The record insertion contains an invalid command. Please try again.\n");
        return;
    }

    // Add the new record to the records array
    records[recordCount].ID = id; // Store the ID
    strncpy(records[recordCount].Name, name, sizeof(records[recordCount].Name) - 1); // Store the name
    records[recordCount].Name[sizeof(records[recordCount].Name) - 1] = '\0'; // Null-terminate the name
    strncpy(records[recordCount].Programme, programme, sizeof(records[recordCount].Programme) - 1); // Store the programme
    records[recordCount].Programme[sizeof(records[recordCount].Programme) - 1] = '\0'; // Null-terminate the programme
    records[recordCount].Mark = mark; // Store the mark

    // Add-on Unique Feature: Add Column
    // Initialise custom columns to default values
    for (int j = 0; j < num_custom_cols; j++) {
        if (strcmp(custom_column[j].type, "int") == 0) {
            records[recordCount].custom_column[j].int_value = 0;
        } else if (strcmp(custom_column[j].type, "float") == 0) {
            records[recordCount].custom_column[j].float_value = 0.0f;
        } else if (strcmp(custom_column[j].type, "string") == 0) {
            records[recordCount].custom_column[j].string_value[0] = '\0';
        }
    }

    // Parse optional custom-column values from the command
    for (int j = 0; j < num_custom_cols; j++) {
        char pattern[64];
        snprintf(pattern, sizeof(pattern), "%s=", custom_column[j].name);

        char *pos = strstr(command, pattern);
        if (!pos) { // User didn't supply this column in the INSERT, keep default
            continue;
        }
        pos += strlen(pattern);
        // Skip leading spaces after '=' if any
        while (isspace((unsigned char)*pos)) {
            pos++;
        }
        // Extract value up to next space or end-of-line
        char valueBuffer[MAX_COLUMN_DATA_LENGTH];
        int k = 0;
        while (*pos != '\0' && !isspace((unsigned char)*pos) && k < MAX_COLUMN_DATA_LENGTH - 1) {
            valueBuffer[k++] = *pos++;
        }
        valueBuffer[k] = '\0';
        // If empty after '=', just leave default
        if (k == 0) {
            continue;
        }
        // Store by type
        if (strcmp(custom_column[j].type, "int") == 0) {
            int intValue;
            if (sscanf(valueBuffer, "%d", &intValue) != 1) {
                printf("CMS: Invalid value '%s' for column %s (expected int).\n", valueBuffer, custom_column[j].name);
                return;
            }
            records[recordCount].custom_column[j].int_value = intValue;
        } else if (strcmp(custom_column[j].type, "float") == 0) {
            float floatValue;
            if (sscanf(valueBuffer, "%f", &floatValue) != 1) {
                printf("CMS: Invalid value '%s' for column %s (expected float).\n",valueBuffer, custom_column[j].name);
                return;
            }
            records[recordCount].custom_column[j].float_value = floatValue;
        } else if (strcmp(custom_column[j].type, "string") == 0) {
            strncpy(records[recordCount].custom_column[j].string_value,valueBuffer, MAX_COLUMN_DATA_LENGTH - 1);
            records[recordCount].custom_column[j].string_value[MAX_COLUMN_DATA_LENGTH - 1] = '\0';
        }
    }
    recordCount++; // Increment the record count
    // Confirm insertion is successful
    printf("CMS: The record with ID=%d is successfully inserted.\n", id);
}


int checkRecordIDExist_operation(int id)
{
    for (int i = 0; i < recordCount; i++)
    {
        if (records[i].ID == id)
        {
            return i;  // Return the index where the ID was found
        }
    }
    return -1;  // Return -1 if ID was not found
}

/*----------------------------------------------------------------
To search if there is any existing record with a given student ID.
-----------------------------------------------------------------*/
void query_operation(char *command)
{
    // Expected format: QUERY ID=<id>

    char *id_ptr = strstr(command, "ID=");
    if (id_ptr == NULL)
    {
        printf("Invalid format. Please use: QUERY ID=<id>\n");
        return;
    }

    id_ptr += 3; // move pointer past "ID="

    // Convert the ID part to a number
    char *endptr;
    long query_ID_value = strtol(id_ptr, &endptr, 10);

    // Check validity: no digits read
    if (endptr == id_ptr)
    {
        printf("Invalid ID. Please ensure only numeric digits are used.\n");
        return;
    }

    // Check remaining characters
    while (*endptr != '\0')
    {
        if (!isspace((unsigned char)*endptr))
        {
            printf("Invalid characters after ID. Only digits allowed.\n");
            return;
        }
        endptr++;
    }

    printf("You entered ID: %ld\n", query_ID_value);

    printf("\n%-10s  %-20s  %-24s  %-5s\n", "ID", "Name", "Programme", "Mark");
    printf("---------------------------------------------------------------------\n");

    int found = 0;

    for (int i = 0; i < recordCount; i++)
    {
        if (db[i].ID == query_ID_value)
        {
            printf("%-10d  %-20s  %-24s  %-5.2f\n",
                   db[i].ID, db[i].Name, db[i].Programme, db[i].Mark);
            found = 1;
            break;
        }
    }

    if (!found)
    {
        printf("Record not found.\n");
    }
}

/*-----------------------------------------------------
To update the data for a record with a given student ID
-------------------------------------------------------*/
void update_operation() {

}

/*----------------------------------------------------------------
To delete the record with a given student ID.
-----------------------------------------------------------------*/
void delete_operation() {

}

/*----------------------------------------------------------------
Save all the current records into the database file.
-----------------------------------------------------------------*/
void save_operation(const char *filename, const StudentRecords *db, int recordCount, newColumn custom_column[], int num_custom_cols) {
    FILE *fptr;
    fptr = fopen(filename, "w");
    if (fptr == NULL){
        fprintf(stderr,"\nError opening the file.");
        return;
    }

    // Write the default headers 
    fprintf(fptr, "Database Name: CMS\n");
    fprintf(fptr, "Authors: Assistant Prof Oran Zane Devilly\n"); 
    fprintf(fptr, "\n"); 
    fprintf(fptr, "Table Name: StudentRecords\n");
    
    // Format and print default column names
    fprintf(fptr, "ID\tName\t\tProgramme\t\tMark");
    
    // Write custom column names
    for (int j = 0; j < num_custom_cols; j++) {
        fprintf(fptr, "\t\t%s", custom_column[j].name);
    }
    fprintf(fptr, "\n");

    int recordsWritten = 0;
     
    if (recordCount > 1 && db != NULL){
        for (int i = 1; i < recordCount; i++) {
            // Writing default record fields
            fprintf(fptr, "%d\t%s\t%s\t%.1f",db[i].ID, db[i].Name, db[i].Programme, db[i].Mark);
            
            // Save custom data according to their types
            for (int j = 0; j < num_custom_cols; j++) {
                fprintf(fptr, "\t");
                if (strcmp(custom_column[j].type, "int") == 0) {
                    fprintf(fptr, "%d", db[i].custom_column[j].int_value);
                } 
                else if (strcmp(custom_column[j].type, "float") == 0) {
                    fprintf(fptr, "%.1f", db[i].custom_column[j].float_value);
                } 
                else if (strcmp(custom_column[j].type, "string") == 0) {
                    fprintf(fptr, "%s", db[i].custom_column[j].string_value);
                }
            }
            fprintf(fptr, "\n");
            recordsWritten++;
         }
    }

    save_column_metafile(custom_column, num_custom_cols);

    if (recordsWritten == recordCount - 1) {
        printf("The database file \"%s\" is successfully saved\n", filename);
    } else {
        printf("Error Saving to File! Only wrote %d out of %d records.", recordsWritten, recordCount - 1);
    }
    fclose(fptr);
}
//Save custom column metadata to separate meta file
void save_column_metafile(newColumn custom_column[], int num_custom_cols) {    
    FILE *fptr = fopen(META_FILENAME, "w");
    if (fptr == NULL) {
        printf("CMS: Error: Cannot create custom column file \"%s\".\n", META_FILENAME);
        return;
    }
    
    // Write number of custom columns
    fprintf(fptr, "%d\n", num_custom_cols);
    
    // Write each column definition
    for (int i = 0; i < num_custom_cols; i++) {
        fprintf(fptr, "%s %s %d\n", 
                custom_column[i].name, 
                custom_column[i].type, 
                custom_column[i].length);
    }
    
    fclose(fptr);
    printf("CMS: Saved %d column definitions to meta file.\n", num_custom_cols);
}
/*----------------------------------
Implement sorting of student records 
-----------------------------------*/
void sort_operation(void)
{
    char input[100];
    char field[20];
    char order[20];

    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
    }

    while (1) // <-- Loop until user enters valid sort choice
    {

        printf("Which order would you like to sort it by?\n");
        printf("  - id asc      (Ascending Student ID)\n");
        printf("  - id desc     (Descending Student ID)\n");
        printf("  - marks asc   (Ascending Marks)\n");
        printf("  - marks desc  (Descending Marks)\n");
        printf("Please type your choice (e.g. \"id asc\"): ");

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("\nInput error.\n");
            continue; // ask again
        }

        // remove trailing newline
        input[strcspn(input, "\n")] = '\0';

        // parse into two words: field + order
        int scanned = sscanf(input, "%19s %19s", field, order);
        if (scanned != 2)
        {
            printf("\nInvalid format. Please type something like \"id asc\" or \"marks desc\".\n");
            continue; // ask again
        }

        // convert to lowercase for case-insensitive comparison
        for (int i = 0; field[i]; i++)
            field[i] = (char)tolower((unsigned char)field[i]);
        for (int i = 0; order[i]; i++)
            order[i] = (char)tolower((unsigned char)order[i]);

        // decide which sort to use
        if (strcmp(field, "id") == 0 && strcmp(order, "asc") == 0)
        {
            // sort by ID (Ascending)
            qsort(db, recordCount - 1, sizeof(StudentRecords), compIDAscend);
            break;
        }
        else if (strcmp(field, "id") == 0 && strcmp(order, "desc") == 0)
        {
            // sort by ID (Descending)
            qsort(db, recordCount - 1, sizeof(StudentRecords), compIDDescend);
            break;
        }
        else if ((strcmp(field, "marks") == 0 || strcmp(field, "mark") == 0) &&
                 strcmp(order, "asc") == 0)
        {
            // sort by Marks (Ascending)
            qsort(db, recordCount - 1, sizeof(StudentRecords), compMarksAscend);
            break;
        }
        else if ((strcmp(field, "marks") == 0 || strcmp(field, "mark") == 0) &&
                 strcmp(order, "desc") == 0)
        {
            // sort by Marks (Descending)
            qsort(db, recordCount - 1, sizeof(StudentRecords), compMarksDescend);
            break;
        }
        else
        {
            printf("\nInvalid choice. Examples of valid inputs:\n");
            printf("  id asc\n");
            printf("  id desc\n");
            printf("  marks asc\n");
            printf("  marks desc\n");
            continue; // ask again
        }
    }

    // show sorted records
    showall_operation();
}
// Custom function to sort the ID (Ascending Order)
int compIDAscend(const void *a, const void *b)
{
    const StudentRecords *student1 = (const StudentRecords *)a;
    const StudentRecords *student2 = (const StudentRecords *)b;
    return student1->ID - student2->ID;
}

int compIDDescend(const void *a, const void *b)
{
    const StudentRecords *student1 = (const StudentRecords *)a;
    const StudentRecords *student2 = (const StudentRecords *)b;
    return student2->ID - student1->ID;
}

// Custom function to sort the Marks (Ascending Order)
int compMarksAscend(const void *a, const void *b)
{
    const StudentRecords *student1 = (const StudentRecords *)a;
    const StudentRecords *student2 = (const StudentRecords *)b;
    return student1->Mark - student2->Mark;
}

// Custom function to sort the Marks (Descending Order)
int compMarksDescend(const void *a, const void *b)
{
    const StudentRecords *student1 = (const StudentRecords *)a;
    const StudentRecords *student2 = (const StudentRecords *)b;
    return student2->Mark - student1->Mark;
}

/*------------------------
Implement summary commands
-------------------------*/
void summary_statics_operation() {
    if (!databaseLoaded) {
        printf("CMS: Database is not loaded. Please load the database first.\n");
        return;
    }

    if (recordCount <= 1) { // Only header exists
        printf("CMS: No student records available.\n");
        return;
    }

    int totalStudents = recordCount - 1;   // recordCount is total lines including header, -1 excludes header and get actual students 
    float sum = 0;

    // Initialize using first student
    float highest = records[1].Mark;
    float lowest = records[1].Mark;
    char highestName[MAX_LENGTH_NAME];
    char lowestName[MAX_LENGTH_NAME];
    strcpy(highestName, records[1].Name);
    strcpy(lowestName, records[1].Name);

    for (int i = 1; i < recordCount; i++) {
        float mark = records[i].Mark;
        sum += mark;

        if (mark > highest) {
            highest = mark;
            strcpy(highestName, records[i].Name);
        }

        if (mark < lowest) {
            lowest = mark;
            strcpy(lowestName, records[i].Name);
        }
    }

    float avg = sum / totalStudents;

    printf("CMS: Summary of Student Records\n");
    printf("Total number of students : %d\n", totalStudents);
    printf("Average mark            : %.2f\n", avg);
    printf("Highest mark            : %.1f (%s)\n", highest, highestName);
    printf("Lowest mark             : %.1f (%s)\n", lowest, lowestName);
}

/*-------------------------------------------
Function to add new columns in the database
--------------------------------------------*/
void add_column_operation(const char* command, newColumn custom_column[], int *num_custom_cols) {
    char colName[MAX_CUSTOM_COL_NAME]; //Name of new column
    char colType[MAX_CUSTOM_COL_NAME]; //Type of new column
    int colLength; //Length of new column

    // Check if number of existing columns exceeds the maximum number of columns specified
    if (*num_custom_cols >= MAX_CUSTOM_COLUMN_NO) {
        printf("CMS: Cannot add column. Maximum number of custom columns (%d) reached.\n", MAX_CUSTOM_COLUMN_NO);
        return;
    }

    // Parse the command for column name, type and length
    int itemsRead = sscanf(command, "ADD COLUMN Name=%49[^ ] Type=%49[^ ] Length=%d", colName, colType, &colLength);

    if (itemsRead != 3) {
        printf("CMS: Invalid ADD COLUMN command format.\n");
        printf("Usage: ADD COLUMN Name=<ColumnName> Type=<ColumnType> Length=<ColumnLength>\n");
        return;
    }
    
    // Validate the column name by length
    if (strlen(colName) >= MAX_CUSTOM_COL_NAME) {
        printf("CMS: Column name '%s' is too long (max %d characters).\n", colName, MAX_CUSTOM_COL_NAME - 1);
        return;
    }

    // Check if the name of column to add already exists 
    if (checkColumnNameExists(colName, custom_column, *num_custom_cols)) {
        printf("CMS: Column name '%s' already exists in the database.\n", colName);
        return;
    }

    // Check if column type is valid
    if (!isValidColumnType(colType)) {
        printf("CMS: Invalid column type '%s'.\n", colType);
        printf("Valid types are: ");
        for (int i = 0; i < NUM_COLUMN_TYPES; i++) {
            printf("%s%s", valid_column_types[i], (i == NUM_COLUMN_TYPES - 1) ? "" : ", ");
        }
        printf(".\n");
        return;
    }

    //Check if input for new column length is between 0 and 100
    if (colLength <= 0 || colLength > 100) { 
        printf("CMS: Column length must be a positive integer and less than or equal to 100.\n");
        return;
    }

    // Add the new column definition
    strcpy(custom_column[*num_custom_cols].name, colName);
    strcpy(custom_column[*num_custom_cols].type, colType);
    custom_column[*num_custom_cols].length = colLength;


    (*num_custom_cols)++; // Increment the counter for number of columns 

    printf("CMS: Successfully added new column: Name='%s', Type='%s', Length='%d'.\n", colName, colType, colLength);
    // Ask user if they want to input data for existing records
    if (databaseLoaded && recordCount > 1) {
        char input[10];
        int validInput = 0;
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        
        while (!validInput) {
            printf("CMS: Do you want to input data for the new column '%s'? (y/n): ", colName);
            
            if (fgets(input, sizeof(input), stdin) != NULL) {
                // Remove newline character
                input[strcspn(input, "\n")] = '\0';
                
                if (input[0] == 'y' || input[0] == 'Y') {
                    input_new_column_data(colName, colType, *num_custom_cols - 1);
                    validInput = 1;
                } 
                else if (input[0] == 'n' || input[0] == 'N') {
                    printf("CMS: No data will be added to records for column '%s'.\n", colName);
                    validInput = 1;
                }
                else {
                    printf("CMS: Invalid input. Please enter 'y' for yes or 'n' for no.\n");
                }
            }
        }
    }
}

//Check column name
int checkColumnNameExists(const char *colName, newColumn custom_column[], int num_custom_cols) {
    // Check with initial default columns
    for (int i = 0; i < NUM_DEFAULT_COLS; i++) {
        if (strcmp(colName, default_column_names[i]) == 0) {
            return 1; // Column name is a reserved default field
        }
    }
    // Check against existing custom columns
    for (int i = 0; i < num_custom_cols; i++) {
        if (strcmp(colName, custom_column[i].name) == 0) {
            return 1; // Column name already added by the user
        }
    }
    return 0; // Does not exist
}

// Check column type
int isValidColumnType(const char *colType) {
    // Check against the array of valid types
    for (int i = 0; i <NUM_COLUMN_TYPES; i++) {
        // Use strcmp for case-sensitive check, adjust if case-insensitivity is preferred
        if (strcmp(colType, valid_column_types[i]) == 0) {
            return 1; // Valid type
        }
    }
    return 0; // Invalid type
}

//Input data for newly added column in existing records
void input_new_column_data(const char *colName, const char *colType, int colIndex) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    printf("CMS: Input data for column '%s' (type: %s) in existing records:\n", colName, colType);
    printf("CMS: Enter to leave record blank.\n");

    for (int i = 1; i < recordCount; i++) { 
        printf("Entering %s Record for ID %d: ", colName, records[i].ID);
        
        char input[100];
        if (fgets(input, sizeof(input), stdin) != NULL) {
            // Remove newline character
            input[strcspn(input, "\n")] = '\0';
            
            // Skip this record if user does not enter anything
            if (strlen(input) == 0) {
                printf("CMS: Skipped record %d.\n\n", records[i].ID);
                
                // Set default value based on type
                if (strcmp(colType, "int") == 0) {
                    records[i].custom_column[colIndex].int_value = 0;
                } else if (strcmp(colType, "float") == 0) {
                    records[i].custom_column[colIndex].float_value = 0.0f;
                } else if (strcmp(colType, "string") == 0) {
                    strcpy(records[i].custom_column[colIndex].string_value, "");
                }
                continue;
            }
            
            // Process input based on column type
            if (strcmp(colType, "int") == 0) {
                int value = atoi(input);
                records[i].custom_column[colIndex].int_value = value;
                printf("CMS: Set %s = %d for record %d.\n\n", colName, value, records[i].ID);
                
            } else if (strcmp(colType, "float") == 0) {
                float value = atof(input);
                records[i].custom_column[colIndex].float_value = value;
                printf("CMS: Set %s = %.2f for record %d.\n\n", colName, value, records[i].ID);
                
            } else if (strcmp(colType, "string") == 0) {
                // Validate string length
                if (strlen(input) > MAX_COLUMN_DATA_LENGTH - 1) {
                    printf("CMS: Warning: Input too long. Truncated to %d characters.\n", MAX_COLUMN_DATA_LENGTH - 1);
                }
                strncpy(records[i].custom_column[colIndex].string_value, input, MAX_COLUMN_DATA_LENGTH - 1);
                records[i].custom_column[colIndex].string_value[MAX_COLUMN_DATA_LENGTH - 1] = '\0';
                printf("CMS: Set %s = '%s' for record %d.\n\n", colName, records[i].custom_column[colIndex].string_value, records[i].ID);
            }
        }
    }
    
    printf("CMS: Successfully input data for column '%s'.\n", colName);
}
