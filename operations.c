#include "./tools/splice.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*-------------------------------------------------
Convert user input for operation choice to uppercase
----------------------------------------------------*/
void toUpperCase(char *str) {
  // Loop through each character in the string until reaching the null
  // terminator
  for (int i = 0; str[i]; i++) {
    // Convert the current character to uppercase using toupper function
    str[i] = toupper(str[i]);
  }
}

/*-------------------------------------------------
Open the database file and read in all the records
--------------------------------------------------*/
void open_operation(const char *filename) {
  // Open the file in read mode
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    // If cannot be found or opened, then display error message
    printf("CMS: Cannot locate file, check file name \"%s\".\n", filename);
    return;
  }

  char line[200];  //  to store 200 chars for each line read from the file (we
                   //  dont expect each line to be more than 200 chars including
                   //  \0)
  recordCount = 0; // Reset the record count
  int isEmpty = 1; // Flag to check if the file has non-whitespace content

  // Verify if the file is empty and show error
  while (fgets(line, sizeof(line), file)) {
    // Verify if the line has any non-whitespace character
    for (int i = 0; line[i] != '\0'; i++) {
      if (!isspace(line[i])) {
        isEmpty = 0; // Found non-whitespace content and Set flag to indicate
                     // the file is not empty
        break;
      }
    }
    if (!isEmpty)
      break; // Exit the loop as soon as we find non-empty content
  }

  if (isEmpty) // If the file is empty
  {
    // Display error message when the file has no content and close the file
    printf("CMS: Your file does not have any contents.\n");
    fclose(file);
    return;
  }

  // Reset file pointer to beginning after checking file is empty which reacches
  // EOF
  rewind(file);

  // Load column definitions
  load_column_metadata(filename, custom_column, &num_custom_cols);

  // Initialize tableName to "Unknown" in case we don't find it (error handling)
  strcpy(tableName, "Unknown <There are no table name>");

  // Read each line until we find the table name
  while (fgets(line, sizeof(line), file)) {
    // Verify if this line contains "Table Name:"
    if (strstr(line, "Table Name:") != NULL) {
      // Move pointer to the text after "Table Name:"
      char *pos = strstr(line, "Table Name:") + strlen("Table Name:");

      // Trim any leading whitespace
      while (isspace(*pos))
        pos++;

      // Copy the table name to the variable
      strncpy(tableName, pos, sizeof(tableName) - 1);
      tableName[strcspn(tableName, "\n")] =
          '\0'; // Remove trailing newline for readability

      break; // Exit loop after finding the table name
    }
  }

  // Add the header as the first "record" in the array
  records[0].ID = 0; // Set ID to 0 or any special value for the header
  strncpy(records[0].Name, "Name",
          sizeof(records[0].Name) - 1); // Set "Name" header
  strncpy(records[0].Programme, "Programme",
          sizeof(records[0].Programme) - 1); // Set "Programme" header
  records[0].Mark = 0.0f;                    // Set "Mark" header

  recordCount = 1; // Start counting number of student records

  // Process each data line
  while (fgets(line, sizeof(line), file)) {
    // Ignore empty lines or header lines in the file
    if (strstr(line, "ID")) {
      continue;
    }

    int id;             // Variable to store the ID
    char name[50];      // Variable to store the name
    char programme[50]; // Variable to store the programme name
    float mark;         // Variable to store the mark

    int matched =
        sscanf(line, "%d %49[^\t] %49[^\t] %f", &id, name, programme, &mark);

    if (matched == 4) {

      records[recordCount].ID = id;
      strncpy(records[recordCount].Name, name,
              sizeof(records[recordCount].Name) - 1);
      strncpy(records[recordCount].Programme, programme,
              sizeof(records[recordCount].Programme) - 1);
      records[recordCount].Mark = mark;

      recordCount++;
    } else {

      printf("CMS: Warning: Could not read line: %s\n", line);
    }

    if (recordCount >= MAX_RECORDS) {
      // Display error message and break out of the loop
      printf("CMS: Warning: Maximum record limit reached.\n");
      break;
    }
  }

  fclose(file);
  databaseLoaded = 1;
  printf("CMS: The database file \"%s\" is successfully opened.\n", filename);
}

// Load column definitions from separate meta file
void load_column_metadata(const char *filename, newColumn custom_column[],
                          int *num_custom_cols) {
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
    if (fscanf(fptr, "%s %s %d", custom_column[i].name, custom_column[i].type,
               &custom_column[i].length) != 3) {
      printf("CMS: Warning: Error reading custom columns %d from meta file.\n",
             i);
      break;
    }
  }

  fclose(fptr);
}

/*----------------------------------------------------------------
Function to To display all the current records in the read-in data
-----------------------------------------------------------------*/
void showall_operation() {
  // Verify if the database is loaded
  if (!databaseLoaded) {
    // Print an error message if the database has not been loaded
    printf("CMS: Database is not loaded. Please load the database first.\n");
    return;
  }

  // Verify if there are any records to display
  if (recordCount > 0) {
    // Print a message indicating the records will be displayed
    printf("CMS: Here are all the records found in the table \"%s\".\n",
           tableName);

    // Initialize variables to determine maximum column widths
    int maxNameLength =
        strlen("Name"); // Start with the length of the "Name" header
    int maxProgrammeLength =
        strlen("Programme"); // Start with the length of the "Programme" header

    // Loop through each record to find the longest name and programme lengths
    for (int i = 0; i < recordCount; i++) {
      int nameLength = strlen(
          records[i].Name); // Get the length of the current record's name
      int programmeLength = strlen(
          records[i]
              .Programme); // Get the length of the current record's programme
      if (nameLength > maxNameLength)
        maxNameLength =
            nameLength; // Update maxNameLength if the current name is longer
      if (programmeLength > maxProgrammeLength)
        maxProgrammeLength = programmeLength; // Update maxProgrammeLength if
                                              // the current programme is longer
    }

    // Create format strings dynamically based on the maximum lengths
    char formatHeader[100], formatRow[100];
    snprintf(formatHeader, sizeof(formatHeader), "%%-8s %%-%ds %%-%ds %%s\n",
             maxNameLength + 3, maxProgrammeLength + 3);
    // Format for header: %-8s for "ID", dynamic width for "Name" and
    // "Programme", and default for "Mark"
    snprintf(formatRow, sizeof(formatRow), "%%-8d %%-%ds %%-%ds %%.1f\n",
             maxNameLength + 3, maxProgrammeLength + 3);
    // Format for rows: %-8d for ID, dynamic width for "Name" and "Programme",
    // and %.1f for "Mark" (1 decimal place)

    // Loop through each record and print it
    for (int i = 0; i < recordCount; i++) {
      if (records[i].ID == 0) // Special case: Header record (ID = 0)
      {
        // Print the header row from the records array (if present)
        printf(formatHeader, "ID", records[i].Name, records[i].Programme,
               "Mark");
      } else {
        // Print the current record's data using the row format
        printf(formatRow,
               records[i].ID,        // Print the ID
               records[i].Name,      // Print the name
               records[i].Programme, // Print the programme
               records[i].Mark);     // Print the mark
      }
    }
  } else {
    // Print an error message if there are no records to display
    printf("CMS: No records available to display.\n");
  }
}

/*--------------------------
To insert a new data record
---------------------------*/
void insert_operation(const char *command) {
  // Verify if the database is loaded
  if (!databaseLoaded) {
    printf("CMS: Database is not loaded. Please load the database first.\n");
    return;
  }

  int id;                   // Variable to store the ID
  char name[50] = {0};      // Array to store the name
  char programme[50] = {0}; // Array to store the programme
  float mark;               // Variable to store the mark

  // Read ID from the command
  if (sscanf(command, "INSERT ID=%d", &id) != 1) {
    // If the command does not contain a valid ID, display an error
    printf("CMS: The record insertion contains an invalid command. Please try "
           "again.\n");
    return;
  }

  // Verify ID length (convert to string to verify length)
  char idStr[20];
  sprintf(idStr, "%d", id);  // Convert the ID to a string
  int idLen = strlen(idStr); // Get the length of the string
  if (idLen != 7) {
    // If the ID is not exactly 7 digits, display an error
    printf("CMS: Student ID must be exactly 7 digits.\n");
    return;
  }

  // Verify if the ID already exists in the records
  if (checkRecordIDExist_operation(id) != -1) {
    // If the ID is found, display a duplicate ID error and exit
    printf("CMS: The record with ID=%d already exists.\n", id);
    return;
  }

  // Verify if the command does not contain additional fields (Name, Programme,
  // Mark)
  if (!strstr(command, "Name=") || !strstr(command, "Programme=") ||
      !strstr(command, "Mark=")) {
    // If additional fields are missing, prompt the user to enter them
    printf("CMS: The record with ID=%d is available. Please enter the details "
           "in the format:\n",
           id);
    printf("Name=<name> Programme=<programme> Mark=<mark>\n");
    return;
  }

  // Read the remaining fields (Name, Programme, Mark)
  char *namePos = strstr(command, "Name="); // Find the position of "Name="
  char *programmePos =
      strstr(command, "Programme=");        // Find the position of "Programme="
  char *markPos = strstr(command, "Mark="); // Find the position of "Mark="

  // If any field is missing or invalid, display an error
  if (!namePos || !programmePos || !markPos) {
    printf("CMS: The record insertion contains an invalid command. Please try "
           "again.\n");
    return;
  }

  // Extract the name field (substring between "Name=" and "Programme=")
  namePos += 5; // Skip "Name="
  int nameLen =
      programmePos - namePos; // Calculate the length of the name substring
  strncpy(name, namePos,
          nameLen - 1);     // Copy the substring to the 'name' variable
  name[nameLen - 1] = '\0'; // Null-terminate the string

  // Extract the programme field (substring between "Programme=" and "Mark=")
  programmePos += 10; // Skip "Programme="
  int programmeLen =
      markPos - programmePos; // Calculate the length of the programme substring
  strncpy(programme, programmePos,
          programmeLen - 1); // Copy the substring to the 'programme' variable
  programme[programmeLen - 1] = '\0'; // Null-terminate the string

  // Extract the mark field (after "Mark=")
  if (sscanf(markPos, "Mark=%f", &mark) != 1) {
    // If the mark field is invalid, display an error
    printf("CMS: The record insertion contains an invalid command. Please try "
           "again.\n");
    return;
  }

  // Validate the name (must contain only letters and spaces)
  for (int i = 0; name[i]; i++) {
    if (!isalpha(name[i]) && !isspace(name[i])) {
      // If the name contains invalid characters, display an error
      printf("CMS: The record insertion contains an invalid command. Please "
             "try again.\n");
      return;
    }
  }

  // Validate the programme (must contain only letters and spaces)
  for (int i = 0; programme[i]; i++) {
    if (!isalpha(programme[i]) && !isspace(programme[i])) {
      // If the programme contains invalid characters, display an error
      printf("CMS: The record insertion contains an invalid command. Please "
             "try again.\n");
      return;
    }
  }

  // Validate the mark (must be between 0 and 100)
  if (mark < 0 || mark > 100) {
    // If the mark is outside the valid range, display an error
    printf("CMS: The record insertion contains an invalid command. Please try "
           "again.\n");
    return;
  }

  // Add the new record to the records array
  records[recordCount].ID = id; // Store the ID
  strncpy(records[recordCount].Name, name,
          sizeof(records[recordCount].Name) - 1); // Store the name
  records[recordCount].Name[sizeof(records[recordCount].Name) - 1] =
      '\0'; // Null-terminate the name
  strncpy(records[recordCount].Programme, programme,
          sizeof(records[recordCount].Programme) - 1); // Store the programme
  records[recordCount].Programme[sizeof(records[recordCount].Programme) - 1] =
      '\0';                         // Null-terminate the programme
  records[recordCount].Mark = mark; // Store the mark
  recordCount++;                    // Increment the record count

  // Confirm insertion is successful
  printf("CMS: The record with ID=%d is successfully inserted.\n", id);
}

int checkRecordIDExist_operation(int id) {
  for (int i = 0; i < recordCount; i++) {
    if (records[i].ID == id) {
      return i; // Return the index where the ID was found
    }
  }
  return -1; // Return -1 if ID was not found
}

/*----------------------------------------------------------------
To search if there is any existing record with a given student ID.
-----------------------------------------------------------------*/
void query_operation() {}

/*-----------------------------------------------------
To update the data for a record with a given student ID
-------------------------------------------------------*/
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

/*----------------------------------------------------------------
To delete the record with a given student ID.
-----------------------------------------------------------------*/
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

/*----------------------------------------------------------------
Save all the current records into the database file.
-----------------------------------------------------------------*/
void save_operation(const char *filename, const StudentRecords *db,
                    int recordCount, newColumn custom_column[],
                    int num_custom_cols) {
  FILE *fptr;
  fptr = fopen(filename, "w");
  if (fptr == NULL) {
    fprintf(stderr, "\nError opening the file.");
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

  if (recordCount > 1 && db != NULL) {
    for (int i = 1; i < recordCount; i++) {
      // Writing default record fields
      fprintf(fptr, "%d\t%s\t%s\t%.1f\t", db[i].ID, db[i].Name, db[i].Programme,
              db[i].Mark);

      // Save custom data according to their types
      for (int j = 0; j < num_custom_cols; j++) {
        fprintf(fptr, "\t");
        if (strcmp(custom_column[j].type, "int") == 0) {
          fprintf(fptr, "%d", db[i].custom_column[j].int_value);
        } else if (strcmp(custom_column[j].type, "float") == 0) {
          fprintf(fptr, "%.1f", db[i].custom_column[j].float_value);
        } else if (strcmp(custom_column[j].type, "string") == 0) {
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
    printf("Error Saving to File! Only wrote %d out of %d records.",
           recordsWritten, recordCount - 1);
  }
  fclose(fptr);
}
// Save custom column metadata to separate meta file
void save_column_metafile(newColumn custom_column[], int num_custom_cols) {
  FILE *fptr = fopen(META_FILENAME, "w");
  if (fptr == NULL) {
    printf("CMS: Error: Cannot create custom column file \"%s\".\n",
           META_FILENAME);
    return;
  }

  // Write number of custom columns
  fprintf(fptr, "%d\n", num_custom_cols);

  // Write each column definition
  for (int i = 0; i < num_custom_cols; i++) {
    fprintf(fptr, "%s %s %d\n", custom_column[i].name, custom_column[i].type,
            custom_column[i].length);
  }

  fclose(fptr);
  printf("CMS: Saved %d column definitions to meta file.\n", num_custom_cols);
}
/*----------------------------------
Implement sorting of student records
-----------------------------------*/
void sort_operation() {}

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

  int totalStudents =
      recordCount - 1; // recordCount is total lines including header, -1
                       // excludes header and get actual students
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
void add_column_operation(const char *command, newColumn custom_column[],
                          int *num_custom_cols) {
  char colName[MAX_CUSTOM_COL_NAME]; // Name of new column
  char colType[MAX_CUSTOM_COL_NAME]; // Type of new column
  int colLength;                     // Length of new column

  // Check if number of existing columns exceeds the maximum number of columns
  // specified
  if (*num_custom_cols >= MAX_CUSTOM_COLUMN_NO) {
    printf("CMS: Cannot add column. Maximum number of custom columns (%d) "
           "reached.\n",
           MAX_CUSTOM_COLUMN_NO);
    return;
  }

  // Parse the command for column name, type and length
  int itemsRead =
      sscanf(command, "ADD COLUMN Name=%49[^ ] Type=%49[^ ] Length=%d", colName,
             colType, &colLength);

  if (itemsRead != 3) {
    printf("CMS: Invalid ADD COLUMN command format.\n");
    printf("Usage: ADD COLUMN Name=<ColumnName> Type=<ColumnType> "
           "Length=<ColumnLength>\n");
    return;
  }

  // Validate the column name by length
  if (strlen(colName) >= MAX_CUSTOM_COL_NAME) {
    printf("CMS: Column name '%s' is too long (max %d characters).\n", colName,
           MAX_CUSTOM_COL_NAME - 1);
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
      printf("%s%s", valid_column_types[i],
             (i == NUM_COLUMN_TYPES - 1) ? "" : ", ");
    }
    printf(".\n");
    return;
  }

  // Check if input for new column length is between 0 and 100
  if (colLength <= 0 || colLength > 100) {
    printf("CMS: Column length must be a positive integer and less than or "
           "equal to 100.\n");
    return;
  }

  // Add the new column definition
  strcpy(custom_column[*num_custom_cols].name, colName);
  strcpy(custom_column[*num_custom_cols].type, colType);
  custom_column[*num_custom_cols].length = colLength;

  (*num_custom_cols)++; // Increment the counter for number of columns

  printf("CMS: Successfully added new column: Name='%s', Type='%s', "
         "Length='%d'.\n",
         colName, colType, colLength);
}

// Check column name
int checkColumnNameExists(const char *colName, newColumn custom_column[],
                          int num_custom_cols) {
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
  for (int i = 0; i < NUM_COLUMN_TYPES; i++) {
    // Use strcmp for case-sensitive check, adjust if case-insensitivity is
    // preferred
    if (strcmp(colType, valid_column_types[i]) == 0) {
      return 1; // Valid type
    }
  }
  return 0; // Invalid type
}
