#define CMS_H

#define MAX_LENGTH_NAME 20
#define MAX_PROGRAMME_NAME 20

typedef struct {
    int ID;
    char Name[MAX_LENGTH_NAME];
    char Programme[MAX_PROGRAMME_NAME];
    float Mark;
} StudentRecords;

int open_operation();
void showall_operation();
void insert_operation();
void query_operation();
void update_operation();
void delete_operation();
void save_operation();
void sort_operation();
void summary_statics_operation();
void unique_operation();
