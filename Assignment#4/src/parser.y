%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

extern int yylex();
extern void yyerror(const char *msg);
ASTNode *root;
%}

%union {
    char *str;
    int bool_val;
    ASTNode *node;
    KeyValuePair *pair;
    KeyValueList *pair_list;
    ASTNodeList *node_list;
}

%token LBRACE RBRACE LBRACKET RBRACKET COLON COMMA
%token TRUE FALSE NULLVAL
%token <str> STRING NUMBER

%type <node> object array value string number boolean null
%type <pair_list> pair_list
%type <pair> pair
%type <node_list> value_list

%%

start: object { root = $1; }

object: LBRACE pair_list RBRACE { $$ = create_object_node($2); }
      | LBRACE RBRACE           { $$ = create_object_node(NULL); }

pair_list: pair                   { $$ = create_pair_list($1, NULL); }
         | pair COMMA pair_list   { $$ = create_pair_list($1, $3); }

pair: STRING COLON value { $$ = create_pair($1, $3); }

array: LBRACKET value_list RBRACKET { $$ = create_array_node($2); }
     | LBRACKET RBRACKET           { $$ = create_array_node(NULL); }

value_list: value                    { $$ = create_node_list($1, NULL); }
          | value COMMA value_list   { $$ = create_node_list($1, $3); }

value: object
     | array
     | string
     | number
     | boolean
     | null

string: STRING { $$ = create_string_node($1); }
number: NUMBER { $$ = create_number_node($1); }
boolean: TRUE  { $$ = create_boolean_node(1); }
       | FALSE { $$ = create_boolean_node(0); }
null: NULLVAL  { $$ = create_null_node(); }

%%

void yyerror(const char *msg) {
    extern int line, column;
    fprintf(stderr, "Error: %s at line %d, column %d\n", msg, line, column);
    exit(1);
}
