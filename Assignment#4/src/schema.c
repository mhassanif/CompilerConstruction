#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "schema.h"

Schema *create_schema() {
    Schema *schema = malloc(sizeof(Schema));
    if (!schema) return NULL;
    schema->tables = NULL;
    schema->next_id = 1;
    return schema;
}

Table *find_table(Schema *schema, const char *name) {
    Table *t = schema->tables;
    while (t) {
        if (strcmp(t->name, name) == 0) return t;
        t = t->next;
    }
    return NULL;
}

Table *create_table(Schema *schema, const char *name) {
    Table *table = malloc(sizeof(Table));
    if (!table) return NULL;
    table->name = strdup(name);
    if (!table->name) {
        free(table);
        return NULL;
    }
    table->columns = NULL;
    table->rows = NULL;
    table->next = schema->tables;
    schema->tables = table;
    return table;
}

Column *add_column(Table *table, const char *name) {
    Column *col = table->columns;
    while (col) {
        if (strcmp(col->name, name) == 0) return col;
        col = col->next;
    }
    col = malloc(sizeof(Column));
    if (!col) return NULL;
    col->name = strdup(name);
    if (!col->name) {
        free(col);
        return NULL;
    }
    col->next = table->columns;
    table->columns = col;
    return col;
}

Row *add_row(Table *table, int id, int num_columns) {
    Row *row = malloc(sizeof(Row));
    if (!row) return NULL;
    row->id = id;
    row->values = calloc(num_columns, sizeof(char *));
    if (!row->values) {
        free(row);
        return NULL;
    }
    row->num_columns = num_columns;
    row->next = table->rows;
    table->rows = row;
    return row;
}

int count_columns(Table *table) {
    int count = 0;
    Column *col = table->columns;
    while (col) {
        count++;
        col = col->next;
    }
    return count;
}

int count_pairs(KeyValueList *pairs) {
    int count = 0;
    while (pairs) {
        count++;
        pairs = pairs->next;
    }
    return count;
}

void process_object(Schema *schema, ASTNode *node, const char *parent_table, int parent_id, const char *table_name) {
    if (!schema || !node || node->type != NODE_OBJECT) return;

    Table *table = find_table(schema, table_name);
    if (!table) table = create_table(schema, table_name);
    if (!table) return;

    // First pass: Add all columns
    KeyValueList *pairs = node->data.pairs;
    while (pairs) {
        char *key = pairs->pair->key;
        add_column(table, key);
        pairs = pairs->next;
    }

    // Allocate row with final column count
    int num_columns = count_columns(table);
    Row *row = add_row(table, schema->next_id++, num_columns);
    if (!row) return;

    // Second pass: Assign values
    pairs = node->data.pairs;
    while (pairs) {
        char *key = pairs->pair->key;
        ASTNode *value = pairs->pair->value;
        int col_idx = 0;
        Column *col = table->columns;
        while (col && strcmp(col->name, key) != 0) {
            col_idx++;
            col = col->next;
        }
        if (!col) continue;

        if (value->type == NODE_OBJECT) {
            char child_table[100];
            snprintf(child_table, 100, "%s_%s", table_name, key);
            process_object(schema, value, table_name, row->id, child_table);
            char fk[100];
            snprintf(fk, 100, "%d", schema->next_id - 1);
            row->values[col_idx] = strdup(fk);
        } else if (value->type == NODE_ARRAY && value->data.elements && value->data.elements->node->type == NODE_OBJECT) {
            char child_table[100];
            snprintf(child_table, 100, "%s_%s", table_name, key);
            process_array_objects(schema, value, table_name, row->id, child_table);
        } else if (value->type == NODE_ARRAY) {
            char junction_table[100];
            snprintf(junction_table, 100, "%s_%s", table_name, key);
            process_array_scalars(schema, value, table_name, row->id, junction_table);
        } else {
            char *val = NULL;
            if (value->type == NODE_STRING && value->data.string_val) {
                val = strdup(value->data.string_val);
            } else if (value->type == NODE_NUMBER && value->data.string_val) {
                val = strdup(value->data.string_val);
            } else if (value->type == NODE_BOOLEAN) {
                val = strdup(value->data.bool_val ? "true" : "false");
            }
            if (val) row->values[col_idx] = val;
        }
        pairs = pairs->next;
    }
}

void process_array_objects(Schema *schema, ASTNode *node, const char *parent_table, int parent_id, const char *table_name) {
    if (!schema || !node || node->type != NODE_ARRAY) return;

    Table *table = find_table(schema, table_name);
    if (!table) table = create_table(schema, table_name);
    if (!table) return;

    add_column(table, "seq");
    add_column(table, "parent_id");

    ASTNodeList *elements = node->data.elements;
    int seq = 0;
    while (elements) {
        ASTNode *el = elements->node;
        if (!el || el->type != NODE_OBJECT) {
            elements = elements->next;
            continue;
        }

        // First pass: Add columns
        KeyValueList *pairs = el->data.pairs;
        while (pairs) {
            add_column(table, pairs->pair->key);
            pairs = pairs->next;
        }

        // Allocate row
        int num_columns = count_columns(table);
        Row *row = add_row(table, schema->next_id++, num_columns);
        if (!row) return;

        // Set seq and parent_id
        row->values[0] = malloc(10);
        snprintf(row->values[0], 10, "%d", seq++);
        row->values[1] = malloc(10);
        snprintf(row->values[1], 10, "%d", parent_id);

        // Second pass: Assign values
        pairs = el->data.pairs;
        while (pairs) {
            char *key = pairs->pair->key;
            ASTNode *value = pairs->pair->value;
            int col_idx = 0;
            Column *col = table->columns;
            while (col && strcmp(col->name, key) != 0) {
                col_idx++;
                col = col->next;
            }
            if (!col) continue;

            char *val = NULL;
            if (value->type == NODE_STRING && value->data.string_val) {
                val = strdup(value->data.string_val);
            } else if (value->type == NODE_NUMBER && value->data.string_val) {
                val = strdup(value->data.string_val);
            } else if (value->type == NODE_BOOLEAN) {
                val = strdup(value->data.bool_val ? "true" : "false");
            }
            if (val) row->values[col_idx] = val;
            pairs = pairs->next;
        }
        elements = elements->next;
    }
}

void process_array_scalars(Schema *schema, ASTNode *node, const char *parent_table, int parent_id, const char *table_name) {
    if (!schema || !node || node->type != NODE_ARRAY) return;

    Table *table = find_table(schema, table_name);
    if (!table) table = create_table(schema, table_name);
    if (!table) return;

    add_column(table, "parent_id");
    add_column(table, "index");
    add_column(table, "value");

    ASTNodeList *elements = node->data.elements;
    int idx = 0;
    while (elements) {
        ASTNode *el = elements->node;
        Row *row = add_row(table, schema->next_id++, 3);
        if (!row) return;

        row->values[0] = malloc(10);
        snprintf(row->values[0], 10, "%d", parent_id);
        row->values[1] = malloc(10);
        snprintf(row->values[1], 10, "%d", idx++);
        char *val = NULL;
        if (el->type == NODE_STRING && el->data.string_val) {
            val = strdup(el->data.string_val);
        } else if (el->type == NODE_NUMBER && el->data.string_val) {
            val = strdup(el->data.string_val);
        } else if (el->type == NODE_BOOLEAN) {
            val = strdup(el->data.bool_val ? "true" : "false");
        }
        if (val) row->values[2] = val;
        elements = elements->next;
    }
}

void process_node(Schema *schema, ASTNode *node, const char *parent_table, int parent_id) {
    if (node && node->type == NODE_OBJECT) {
        process_object(schema, node, parent_table, parent_id, "root");
    }
}

void write_csv_files(Schema *schema, const char *out_dir) {
    if (!schema) return;
    Table *table = schema->tables;
    while (table) {
        char filepath[256];
        snprintf(filepath, 256, "%s/%s.csv", out_dir, table->name);
        FILE *fp = fopen(filepath, "w");
        if (!fp) {
            fprintf(stderr, "Error: Cannot open %s\n", filepath);
            exit(1);
        }

        // Write header
        fprintf(fp, "id");
        Column *col = table->columns;
        while (col) {
            fprintf(fp, ",%s", col->name);
            col = col->next;
        }
        fprintf(fp, "\n");

        // Write rows
        Row *row = table->rows;
        while (row) {
            fprintf(fp, "%d", row->id);
            for (int i = 0; i < row->num_columns; i++) {
                fprintf(fp, ",%s", row->values[i] ? row->values[i] : "");
            }
            fprintf(fp, "\n");
            row = row->next;
        }
        fclose(fp);
        table = table->next;
    }
}

void print_csv_to_terminal(Schema *schema) {
    if (!schema) return;
    Table *table = schema->tables;
    while (table) {
        // Print table name (optional, for clarity)
        printf("Table: %s\n", table->name);

        // Print header
        printf("id");
        Column *col = table->columns;
        while (col) {
            printf(",%s", col->name);
            col = col->next;
        }
        printf("\n");

        // Print rows
        Row *row = table->rows;
        while (row) {
            printf("%d", row->id);
            for (int i = 0; i < row->num_columns; i++) {
                printf(",%s", row->values[i] ? row->values[i] : "");
            }
            printf("\n");
            row = row->next;
        }

        // Separate tables with a blank line
        if (table->next) printf("\n");
        table = table->next;
    }
}

void free_schema(Schema *schema) {
    if (!schema) return;
    Table *table = schema->tables;
    while (table) {
        Table *next_table = table->next;
        free(table->name);

        Column *col = table->columns;
        while (col) {
            Column *next_col = col->next;
            free(col->name);
            free(col);
            col = next_col;
        }

        Row *row = table->rows;
        while (row) {
            Row *next_row = row->next;
            for (int i = 0; i < row->num_columns; i++) {
                if (row->values[i]) {
                    free(row->values[i]);
                    row->values[i] = NULL;
                }
            }
            free(row->values);
            free(row);
            row = next_row;
        }
        free(table);
        table = next_table;
    }
    free(schema);
}
