#ifndef AST_H
#define AST_H

typedef enum {
    AST_PROGRAM,
    AST_STATEMENT_LIST,
    AST_VARIABLE_DECLARATION,
    AST_FUNCTION_DEFINITION,
    AST_PARAMETER,
    AST_BLOCK,
    AST_IF_STATEMENT,
    AST_IF_ELSE_STATEMENT,
    AST_WHILE_STATEMENT,
    AST_FOR_STATEMENT,
    AST_RETURN,
    AST_ASSIGNMENT,
    AST_FUNCTION_CALL,
    AST_IDENTIFIER,
    AST_INTEGER,
    AST_FLOATING,
    AST_CHARACTER,
    AST_STRING,
    AST_ADD,
    AST_SUBTRACT,
    AST_MULTIPLY,
    AST_DIVIDE,
    AST_MODULO,
    AST_EQUAL,
    AST_NOT_EQUAL,
    AST_LESS,
    AST_LESS_EQUAL,
    AST_GREATER,
    AST_GREATER_EQUAL,
    AST_LOGICAL_AND,
    AST_LOGICAL_OR,
    AST_LOGICAL_NOT,
    AST_UNARY_MINUS
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    int value;
    char* name;
    struct ASTNode* children;
    struct ASTNode* next;
    struct ASTNode* right;
} ASTNode;

/* Function declarations */
ASTNode* create_node(ASTNodeType type, int value, char* name);
ASTNode* create_binary_node(ASTNodeType type, ASTNode* left, ASTNode* right);
ASTNode* create_unary_node(ASTNodeType type, ASTNode* operand);
void print_ast(ASTNode* node, int depth);
void free_ast(ASTNode* node);
void generate_code(ASTNode* node, FILE* output);
const char* get_node_type_name(ASTNodeType type);

#endif
