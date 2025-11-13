#ifndef CMS_H //include guards to prevent multiple definition of the Student Records structure, enabling contents of cms.h to be copied to main.c and operations.c once 
#define CMS_H

#define MAX_LENGTH_NAME 20
#define MAX_PROGRAMME_NAME 20
#define MAX_CUSTOM_COLUMN_NO 5 //Max number of column user can add
#define MAX_CUSTOM_COL_NAME 20 //Max length of column name
#define MAX_COLUMN_DATA_LENGTH 20 //Max length of data in column that user added

typedef struct {
    int ID;
    char Name[MAX_LENGTH_NAME];
    char Programme[MAX_PROGRAMME_NAME];
    float Mark;
    char custom_column[MAX_CUSTOM_COLUMN_NO][MAX_COLUMN_DATA_LENGTH];
} StudentRecords;

typedef struct {
    char name[MAX_CUSTOM_COL_NAME]; //new column name
    char type[MAX_CUSTOM_COL_NAME]; // new column data type
} newColumn;

extern newColumn custom_column[MAX_CUSTOM_COLUMN_NO];
extern int num_custom_cols;

int open_operation();
void showall_operation();
void insert_operation();
void query_operation();
void update_operation();
void delete_operation();
void save_operation(const char *filename, const StudentRecords *db, int count, newColumn custom_column[], int num_custom_cols);
void sort_operation();
void summary_statics_operation();
void add_column_operation(StudentRecords *db, int recordCount, newColumn custom_column[], int *num_custom_cols);
#endif