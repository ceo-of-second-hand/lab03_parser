%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

extern int yylval;
extern char* yytext;
extern int yylineno;

int yylex(void);
void yyerror(const char* s);

ASTNode* create_node(ASTNodeType type, int value, char* name);
ASTNode* create_binary_node(ASTNodeType type, ASTNode* left, ASTNode* right);
ASTNode* create_unary_node(ASTNodeType type, ASTNode* operand);
void print_ast(ASTNode* node, int depth);
void free_ast(ASTNode* node);
void generate_code(ASTNode* node, FILE* output);
%}

%union {
    int int_val;
    char* string_val;
    ASTNode* node;
}

%token <int_val> INTEGER FLOATING CHARACTER
%token <string_val> IDENTIFIER STRING
%token INT FLOAT CHAR VOID
%token IF ELSE WHILE FOR RETURN BREAK CONTINUE
%token PLUS MINUS MULTIPLY DIVIDE MODULO
%token ASSIGN EQUAL NOT_EQUAL LESS LESS_EQUAL GREATER GREATER_EQUAL
%token AND OR NOT
%token LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET
%token SEMICOLON COMMA

%type <node> program statement_list statement expression
%type <node> declaration assignment if_statement while_statement for_statement
%type <node> return_statement function_call function_definition
%type <node> parameter_list parameter
%type <node> block
%type <node> arithmetic_expression logical_expression
%type <node> primary_expression

%left OR
%left AND
%left EQUAL NOT_EQUAL LESS LESS_EQUAL GREATER GREATER_EQUAL
%left PLUS MINUS
%left MULTIPLY DIVIDE MODULO
%right NOT
%right ASSIGN

%%

program:
    statement_list
    {
        $$ = create_node(AST_PROGRAM, 0, "program");
        $$->children = $1;
        printf("Parsed program successfully\n");
    }
    ;

statement_list:
    statement_list statement
    {
        $$ = create_node(AST_STATEMENT_LIST, 0, "statement_list");
        $$->children = $1;
        $$->next = $2;
    }
    | statement
    {
        $$ = $1;
    }
    ;

statement:
    declaration SEMICOLON
    {
        $$ = $1;
    }
    | assignment SEMICOLON
    {
        $$ = $1;
    }
    | if_statement
    {
        $$ = $1;
    }
    | while_statement
    {
        $$ = $1;
    }
    | for_statement
    {
        $$ = $1;
    }
    | return_statement SEMICOLON
    {
        $$ = $1;
    }
    | function_definition
    {
        $$ = $1;
    }
    | block
    {
        $$ = $1;
    }
    | expression SEMICOLON
    {
        $$ = $1;
    }
    ;

declaration:
    INT IDENTIFIER
    {
        $$ = create_node(AST_VARIABLE_DECLARATION, 0, $2);
    }
    | FLOAT IDENTIFIER
    {
        $$ = create_node(AST_VARIABLE_DECLARATION, 0, $2);
    }
    | CHAR IDENTIFIER
    {
        $$ = create_node(AST_VARIABLE_DECLARATION, 0, $2);
    }
    | INT IDENTIFIER ASSIGN expression
    {
        $$ = create_binary_node(AST_ASSIGNMENT, 
            create_node(AST_IDENTIFIER, 0, $2), $4);
    }
    | FLOAT IDENTIFIER ASSIGN expression
    {
        $$ = create_binary_node(AST_ASSIGNMENT, 
            create_node(AST_IDENTIFIER, 0, $2), $4);
    }
    | CHAR IDENTIFIER ASSIGN expression
    {
        $$ = create_binary_node(AST_ASSIGNMENT, 
            create_node(AST_IDENTIFIER, 0, $2), $4);
    }
    ;

assignment:
    IDENTIFIER ASSIGN expression
    {
        $$ = create_binary_node(AST_ASSIGNMENT, 
            create_node(AST_IDENTIFIER, 0, $1), $3);
    }
    ;

if_statement:
    IF LPAREN expression RPAREN statement
    {
        $$ = create_node(AST_IF_STATEMENT, 0, "if");
        $$->children = $3;
        $$->next = $5;
    }
    | IF LPAREN expression RPAREN statement ELSE statement
    {
        $$ = create_node(AST_IF_ELSE_STATEMENT, 0, "if_else");
        $$->children = $3;
        $$->next = $5;
        $$->right = $7;
    }
    ;

while_statement:
    WHILE LPAREN expression RPAREN statement
    {
        $$ = create_node(AST_WHILE_STATEMENT, 0, "while");
        $$->children = $3;
        $$->next = $5;
    }
    ;

for_statement:
    FOR LPAREN assignment SEMICOLON expression SEMICOLON assignment RPAREN statement
    {
        $$ = create_node(AST_FOR_STATEMENT, 0, "for");
        $$->children = $3;
        $$->next = $5;
        $$->right = $7;
        $$->right->next = $9;
    }
    ;

return_statement:
    RETURN expression
    {
        $$ = create_unary_node(AST_RETURN, $2);
    }
    | RETURN
    {
        $$ = create_node(AST_RETURN, 0, "return");
    }
    ;

function_definition:
    INT IDENTIFIER LPAREN parameter_list RPAREN block
    {
        $$ = create_node(AST_FUNCTION_DEFINITION, 0, $2);
        $$->children = $4;
        $$->next = $6;
    }
    | VOID IDENTIFIER LPAREN parameter_list RPAREN block
    {
        $$ = create_node(AST_FUNCTION_DEFINITION, 0, $2);
        $$->children = $4;
        $$->next = $6;
    }
    ;

parameter_list:
    parameter_list COMMA parameter
    {
        $$ = $1;
        ASTNode* temp = $1;
        while (temp->next) temp = temp->next;
        temp->next = $3;
    }
    | parameter
    {
        $$ = $1;
    }
    |
    {
        $$ = NULL;
    }
    ;

parameter:
    INT IDENTIFIER
    {
        $$ = create_node(AST_PARAMETER, 0, $2);
    }
    | FLOAT IDENTIFIER
    {
        $$ = create_node(AST_PARAMETER, 0, $2);
    }
    | CHAR IDENTIFIER
    {
        $$ = create_node(AST_PARAMETER, 0, $2);
    }
    ;

block:
    LBRACE statement_list RBRACE
    {
        $$ = create_node(AST_BLOCK, 0, "block");
        $$->children = $2;
    }
    ;

expression:
    logical_expression
    {
        $$ = $1;
    }
    ;

logical_expression:
    logical_expression OR arithmetic_expression
    {
        $$ = create_binary_node(AST_LOGICAL_OR, $1, $3);
    }
    | logical_expression AND arithmetic_expression
    {
        $$ = create_binary_node(AST_LOGICAL_AND, $1, $3);
    }
    | arithmetic_expression
    {
        $$ = $1;
    }
    ;

arithmetic_expression:
    arithmetic_expression PLUS arithmetic_expression
    {
        $$ = create_binary_node(AST_ADD, $1, $3);
    }
    | arithmetic_expression MINUS arithmetic_expression
    {
        $$ = create_binary_node(AST_SUBTRACT, $1, $3);
    }
    | arithmetic_expression MULTIPLY arithmetic_expression
    {
        $$ = create_binary_node(AST_MULTIPLY, $1, $3);
    }
    | arithmetic_expression DIVIDE arithmetic_expression
    {
        $$ = create_binary_node(AST_DIVIDE, $1, $3);
    }
    | arithmetic_expression MODULO arithmetic_expression
    {
        $$ = create_binary_node(AST_MODULO, $1, $3);
    }
    | arithmetic_expression EQUAL arithmetic_expression
    {
        $$ = create_binary_node(AST_EQUAL, $1, $3);
    }
    | arithmetic_expression NOT_EQUAL arithmetic_expression
    {
        $$ = create_binary_node(AST_NOT_EQUAL, $1, $3);
    }
    | arithmetic_expression LESS arithmetic_expression
    {
        $$ = create_binary_node(AST_LESS, $1, $3);
    }
    | arithmetic_expression LESS_EQUAL arithmetic_expression
    {
        $$ = create_binary_node(AST_LESS_EQUAL, $1, $3);
    }
    | arithmetic_expression GREATER arithmetic_expression
    {
        $$ = create_binary_node(AST_GREATER, $1, $3);
    }
    | arithmetic_expression GREATER_EQUAL arithmetic_expression
    {
        $$ = create_binary_node(AST_GREATER_EQUAL, $1, $3);
    }
    | primary_expression
    {
        $$ = $1;
    }
    ;

primary_expression:
    INTEGER
    {
        $$ = create_node(AST_INTEGER, $1, NULL);
    }
    | FLOATING
    {
        $$ = create_node(AST_FLOATING, $1, NULL);
    }
    | CHARACTER
    {
        $$ = create_node(AST_CHARACTER, $1, NULL);
    }
    | STRING
    {
        $$ = create_node(AST_STRING, 0, (char*)$1);
    }
    | IDENTIFIER
    {
        $$ = create_node(AST_IDENTIFIER, 0, $1);
    }
    | IDENTIFIER LPAREN RPAREN
    {
        $$ = create_node(AST_FUNCTION_CALL, 0, $1);
    }
    | IDENTIFIER LPAREN parameter_list RPAREN
    {
        $$ = create_node(AST_FUNCTION_CALL, 0, $1);
        $$->children = $3;
    }
    | LPAREN expression RPAREN
    {
        $$ = $2;
    }
    | MINUS primary_expression
    {
        $$ = create_unary_node(AST_UNARY_MINUS, $2);
    }
    | NOT primary_expression
    {
        $$ = create_unary_node(AST_LOGICAL_NOT, $2);
    }
    ;

%%

void yyerror(const char* s) {
    printf("Error at line %d: %s\n", yylineno, s);
}
