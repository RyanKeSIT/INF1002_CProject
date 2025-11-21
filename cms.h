#ifndef CMS_H //include guards to prevent multiple definition of the Student Records structure, enabling contents of cms.h to be copied to main.c and operations.c once 
#define CMS_H

/*---------------
 Define Constants
----------------*/
#define DB_FILENAME "P3_4-CMS.txt" // Default filename
#define META_FILENAME "customcolumn.meta" //Meta file name of custom column

#define MAX_ID_LENGTH 9 //Max length of ID 
#define MAX_LENGTH_NAME 15 // Max length of student name
#define MAX_PROGRAMME_NAME 25 //Max length of programme name
#define MAX_MARK_LENGTH 6 //Max length of mark

#define NUM_DEFAULT_COLS 4 // Initial number of columns
#define MAX_CUSTOM_COLUMN_NO 5 // Max number of column user can add
#define MAX_CUSTOM_COL_NAME 20 // Max length of column name
#define MAX_COLUMN_DATA_LENGTH 20 // Max length of data in column that user added
#define MAX_RECORDS 100 //Max number of records in database 
#define NUM_COLUMN_TYPES 3 // Number of valid column types

/*------------------------------
 Union for Data in Custom Column 
-------------------------------*/
typedef union {
    int int_value;
    float float_value;
    char string_value[MAX_COLUMN_DATA_LENGTH];
} ColumnData;

/*----------------------------------
 Structure for New Columns
----------------------------------*/
typedef struct {
    char name[MAX_CUSTOM_COL_NAME]; //new column name
    char type[MAX_CUSTOM_COL_NAME]; // new column data type
    int length; // new column data type
} newColumn;
newColumn custom_column[MAX_CUSTOM_COLUMN_NO]; // Array to store all the new columns

/*----------------------------
 Structure for Student Records
------------------------------*/
typedef struct {
    int ID;
    char Name[MAX_LENGTH_NAME];
    char Programme[MAX_PROGRAMME_NAME];
    float Mark;
    ColumnData custom_column[MAX_CUSTOM_COLUMN_NO];
} StudentRecords;

StudentRecords records[MAX_RECORDS]; // Array to store student records
StudentRecords *db = NULL;

/*---------------
Variables Definition
------------------*/
int databaseLoaded = 0; // Flag to check if the database is loaded
int recordCount = 0; // Counter for the number of records
char tableName[50]; // Variable to store table name from database
char command[200]; // Buffer to store user input commands
char filename[] = DB_FILENAME; // Variable for the database filename
int num_custom_cols = 0; // Number of custom columns added

const char *default_column_names[NUM_DEFAULT_COLS] = {"ID", "Name", "Programme", "Mark"};
const char *valid_column_types[NUM_COLUMN_TYPES] = {"int", "float", "string"};

/*---------------------
 Core Function Prototypes
----------------------*/
void open_operation(const char *filename);
void showall_operation();
void insert_operation(const char* command);
void query_operation();
void update_operation(char *command);
void delete_operation(char *command);
void save_operation(const char *filename, const StudentRecords *db, int count, newColumn custom_column[], int num_custom_cols);
void sort_operation();
void summary_statics_operation();
void add_column_operation(const char* command, newColumn custom_column[], int *num_custom_cols);

/*-------------------------------------------------------
 Prototypes of Other Functions Required by Core Functions
--------------------------------------------------------*/
void toUpperCase(char *str); // Convert commandPrefix to uppercase
int checkRecordIDExist_operation(int id); // For insert operation
int checkColumnNameExists(const char *colName, newColumn custom_column[], int num_custom_cols); //For add column operation
int isValidColumnType(const char *colType); //For add column operation
void save_column_metafile(newColumn custom_column[], int num_custom_cols);
void load_column_metadata(const char *filename, newColumn custom_column[], int *num_custom_cols);
#endif
