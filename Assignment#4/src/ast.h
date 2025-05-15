#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    NODE_OBJECT,
    NODE_ARRAY,
    NODE_STRING,
    NODE_NUMBER,
    NODE_BOOLEAN,
    NODE_NULL
} NodeType;

// Forward declarations
typedef struct ASTNode ASTNode;
typedef struct KeyValuePair KeyValuePair;
typedef struct KeyValueList KeyValueList;
typedef struct ASTNodeList ASTNodeList;

// Structure for key-value pairs
struct KeyValuePair {
    char *key; // String key (owns memory)
    ASTNode *value;
};

// Linked list of key-value pairs
struct KeyValueList {
    KeyValuePair *pair;
    KeyValueList *next;
};

// Linked list of array elements
struct ASTNodeList {
    ASTNode *node;
    ASTNodeList *next;
};

// AST node structure
struct ASTNode {
    NodeType type;
    union {
        KeyValueList *pairs; // For NODE_OBJECT
        ASTNodeList *elements; // For NODE_ARRAY
        char *string_val; // For NODE_STRING, NODE_NUMBER
        int bool_val; // For NODE_BOOLEAN
    } data;
};

// Function prototypes
ASTNode *create_object_node(KeyValueList *pairs);
ASTNode *create_array_node(ASTNodeList *elements);
ASTNode *create_string_node(char *value);
ASTNode *create_number_node(char *value);
ASTNode *create_boolean_node(int value);
ASTNode *create_null_node();
KeyValuePair *create_pair(char *key, ASTNode *value);
KeyValueList *create_pair_list(KeyValuePair *pair, KeyValueList *next);
ASTNodeList *create_node_list(ASTNode *node, ASTNodeList *next);
void free_ast(ASTNode *node);
void print_ast(ASTNode *node, int indent);

#endif
