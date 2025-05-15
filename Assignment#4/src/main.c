#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "schema.h"
#include "parser.tab.h"

extern FILE *yyin;
extern ASTNode *root;

int main(int argc, char *argv[]) {
    int should_print_ast = 0;
    int should_print_csv = 0; 
    char *out_dir = ".";

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--print-ast") == 0) {
            should_print_ast = 1;
        } else if (strcmp(argv[i], "--print-csv") == 0) {
            should_print_csv = 1;
        } else if (strcmp(argv[i], "--out-dir") == 0 && i + 1 < argc) {
            out_dir = argv[++i];
        }
    }

    yyin = stdin;
    if (yyparse()) {
        return 1;
    }

    if (should_print_ast) {
        print_ast(root, 0);
    }

    Schema *schema = create_schema();
    process_node(schema, root, NULL, 0);

    if (should_print_csv) {
        print_csv_to_terminal(schema); 
    } else {
        write_csv_files(schema, out_dir); 
    }

    free_schema(schema);
    free_ast(root);

    return 0;
}
