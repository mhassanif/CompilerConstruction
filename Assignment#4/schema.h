#ifndef SCHEMA_H
#define SCHEMA_H

#include "ast.h"

typedef struct Column {
    char *name;
    struct Column *next;
} Column;

typedef struct Row {
    int id;
    char **values;
    int num_columns;
    struct Row *next;
} Row;

typedef struct Table {
    char *name;
    Column *columns;
    Row *rows;
    struct Table *next;
} Table;

typedef struct Schema {
    Table *tables;
    int next_id;
} Schema;

Schema *create_schema();
Table *find_table(Schema *schema, const char *name);
Table *create_table(Schema *schema, const char *name);
Column *add_column(Table *table, const char *name);
Row *add_row(Table *table, int id, int num_columns);
int count_columns(Table *table);
void process_node(Schema *schema, ASTNode *node, const char *parent_table, int parent_id);
void process_object(Schema *schema, ASTNode *node, const char *parent_table, int parent_id, const char *table_name);
void process_array_objects(Schema *schema, ASTNode *node, const char *parent_table, int parent_id, const char *table_name);
void process_array_scalars(Schema *schema, ASTNode *node, const char *parent_table, int parent_id, const char *table_name);
void write_csv_files(Schema *schema, const char *out_dir);
void print_csv_to_terminal(Schema *schema); // New declaration
void free_schema(Schema *schema);

#endif
