#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"
#include "ast.h"

extern FILE* yyin;
extern int yyparse();
extern ASTNode* root;

int main(int argc, char* argv[]) {
    printf("=== C Language Parser ===\n");
    printf("Author: Student\n");
    printf("Features: Lexical analysis, syntax parsing, AST generation, code generation\n\n");
    
    if (argc < 2) {
        printf("Usage: %s <input_file> [output_file]\n", argv[0]);
        printf("Example: %s test.c output.c\n", argv[0]);
        return 1;
    }
    
    yyin = fopen(argv[1], "r");
    if (!yyin) {
        printf("Error: Cannot open input file '%s'\n", argv[1]);
        return 1;
    }
    
    printf("Parsing file: %s\n", argv[1]);
    
    int result = yyparse();
    
    if (result == 0) {
        printf("Parsing completed successfully!\n\n");
        printf("=== Abstract Syntax Tree ===\n");
        print_ast(root, 0);
        printf("\n");
        
        char* output_filename = (argc > 2) ? argv[2] : "output.c";
        FILE* output_file = fopen(output_filename, "w");
        if (output_file) {
            printf("Generating code to: %s\n", output_filename);
            generate_code(root, output_file);
            fclose(output_file);
            printf("Code generation completed!\n");
        } else {
            printf("Error: Cannot create output file '%s'\n", output_filename);
        }
        
        free_ast(root);
    } else {
        printf("Parsing failed!\n");
    }
    
    fclose(yyin);
    return result;
}
