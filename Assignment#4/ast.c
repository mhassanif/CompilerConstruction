#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

// Creation functions
ASTNode *create_object_node(KeyValueList *pairs) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = NODE_OBJECT;
    node->data.pairs = pairs;
    return node;
}

ASTNode *create_array_node(ASTNodeList *elements) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = NODE_ARRAY;
    node->data.elements = elements;
    return node;
}

ASTNode *create_string_node(char *value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = NODE_STRING;
    node->data.string_val = value; // Takes ownership
    return node;
}

ASTNode *create_number_node(char *value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = NODE_NUMBER;
    node->data.string_val = value; // Takes ownership
    return node;
}

ASTNode *create_boolean_node(int value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = NODE_BOOLEAN;
    node->data.bool_val = value;
    return node;
}

ASTNode *create_null_node() {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    node->type = NODE_NULL;
    return node;
}

KeyValuePair *create_pair(char *key, ASTNode *value) {
    KeyValuePair *pair = malloc(sizeof(KeyValuePair));
    if (!pair) return NULL;
    pair->key = key;
    pair->value = value;
    return pair;
}

KeyValueList *create_pair_list(KeyValuePair *pair, KeyValueList *next) {
    KeyValueList *list = malloc(sizeof(KeyValueList));
    if (!list) return NULL;
    list->pair = pair;
    list->next = next;
    return list;
}

ASTNodeList *create_node_list(ASTNode *node, ASTNodeList *next) {
    ASTNodeList *list = malloc(sizeof(ASTNodeList));
    if (!list) return NULL;
    list->node = node;
    list->next = next;
    return list;
}

// Free AST function
void free_ast(ASTNode *node) {
    if (!node) return;
    switch (node->type) {
        case NODE_OBJECT: {
            KeyValueList *p = node->data.pairs;
            while (p) {
                KeyValueList *next = p->next;
                if (p->pair) {
                    free(p->pair->key);
                    free_ast(p->pair->value);
                    free(p->pair);
                }
                free(p);
                p = next;
            }
            break;
        }
        case NODE_ARRAY: {
            ASTNodeList *e = node->data.elements;
            while (e) {
                ASTNodeList *next = e->next;
                free_ast(e->node);
                free(e);
                e = next;
            }
            break;
        }
        case NODE_STRING:
        case NODE_NUMBER:
            free(node->data.string_val);
            break;
        case NODE_BOOLEAN:
        case NODE_NULL:
            break;
    }
    free(node);
}

// Print AST function
void print_ast(ASTNode *node, int indent) {
    if (!node) return;
    for (int i = 0; i < indent; i++) printf("  ");
    switch (node->type) {
        case NODE_OBJECT:
            printf("OBJECT\n");
            for (KeyValueList *p = node->data.pairs; p; p = p->next) {
                for (int i = 0; i < indent + 1; i++) printf("  ");
                printf("KEY: %s\n", p->pair->key);
                print_ast(p->pair->value, indent + 2);
            }
            break;
        case NODE_ARRAY:
            printf("ARRAY\n");
            for (ASTNodeList *e = node->data.elements; e; e = e->next) {
                print_ast(e->node, indent + 1);
            }
            break;
        case NODE_STRING:
            printf("STRING: %s\n", node->data.string_val);
            break;
        case NODE_NUMBER:
            printf("NUMBER: %s\n", node->data.string_val);
            break;
        case NODE_BOOLEAN:
            printf("BOOLEAN: %s\n", node->data.bool_val ? "true" : "false");
            break;
        case NODE_NULL:
            printf("NULL\n");
            break;
    }
}
