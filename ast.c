#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

ASTNode* create_node(ASTNodeType type, int value, char* name) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) {
        printf("Memory allocation failed\n");
        return NULL;
    }
    
    node->type = type;
    node->value = value;
    node->name = name ? strdup(name) : NULL;
    node->children = NULL;
    node->next = NULL;
    node->right = NULL;
    
    return node;
}

ASTNode* create_binary_node(ASTNodeType type, ASTNode* left, ASTNode* right) {
    ASTNode* node = create_node(type, 0, NULL);
    if (node) {
        node->children = left;
        if (left) {
            left->next = right;
        }
    }
    return node;
}

ASTNode* create_unary_node(ASTNodeType type, ASTNode* operand) {
    ASTNode* node = create_node(type, 0, NULL);
    if (node) {
        node->children = operand;
    }
    return node;
}

const char* get_node_type_name(ASTNodeType type) {
    switch (type) {
        case AST_PROGRAM: return "PROGRAM";
        case AST_STATEMENT_LIST: return "STATEMENT_LIST";
        case AST_VARIABLE_DECLARATION: return "VARIABLE_DECLARATION";
        case AST_FUNCTION_DEFINITION: return "FUNCTION_DEFINITION";
        case AST_PARAMETER: return "PARAMETER";
        case AST_BLOCK: return "BLOCK";
        case AST_IF_STATEMENT: return "IF_STATEMENT";
        case AST_IF_ELSE_STATEMENT: return "IF_ELSE_STATEMENT";
        case AST_WHILE_STATEMENT: return "WHILE_STATEMENT";
        case AST_FOR_STATEMENT: return "FOR_STATEMENT";
        case AST_RETURN: return "RETURN";
        case AST_ASSIGNMENT: return "ASSIGNMENT";
        case AST_FUNCTION_CALL: return "FUNCTION_CALL";
        case AST_IDENTIFIER: return "IDENTIFIER";
        case AST_INTEGER: return "INTEGER";
        case AST_FLOATING: return "FLOATING";
        case AST_CHARACTER: return "CHARACTER";
        case AST_STRING: return "STRING";
        case AST_ADD: return "ADD";
        case AST_SUBTRACT: return "SUBTRACT";
        case AST_MULTIPLY: return "MULTIPLY";
        case AST_DIVIDE: return "DIVIDE";
        case AST_MODULO: return "MODULO";
        case AST_EQUAL: return "EQUAL";
        case AST_NOT_EQUAL: return "NOT_EQUAL";
        case AST_LESS: return "LESS";
        case AST_LESS_EQUAL: return "LESS_EQUAL";
        case AST_GREATER: return "GREATER";
        case AST_GREATER_EQUAL: return "GREATER_EQUAL";
        case AST_LOGICAL_AND: return "LOGICAL_AND";
        case AST_LOGICAL_OR: return "LOGICAL_OR";
        case AST_LOGICAL_NOT: return "LOGICAL_NOT";
        case AST_UNARY_MINUS: return "UNARY_MINUS";
        default: return "UNKNOWN";
    }
}

void print_ast(ASTNode* node, int depth) {
    if (!node) return;
    
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    
    printf("%s", get_node_type_name(node->type));
    
    if (node->name) {
        printf(" (%s)", node->name);
    }
    if (node->value != 0) {
        printf(" [%d]", node->value);
    }
    printf("\n");
    
    if (node->children) {
        print_ast(node->children, depth + 1);
    }
    if (node->next) {
        print_ast(node->next, depth);
    }
    if (node->right) {
        print_ast(node->right, depth + 1);
    }
}

void generate_code(ASTNode* node, FILE* output) {
    if (!node) return;
    
    switch (node->type) {
        case AST_PROGRAM:
            fprintf(output, "#include <stdio.h>\n");
            fprintf(output, "#include <stdlib.h>\n\n");
            generate_code(node->children, output);
            break;
            
        case AST_FUNCTION_DEFINITION:
            if (node->name) {
                fprintf(output, "int %s(", node->name);
                ASTNode* param = node->children;
                int first = 1;
                while (param) {
                    if (!first) fprintf(output, ", ");
                    fprintf(output, "int %s", param->name);
                    param = param->next;
                    first = 0;
                }
                fprintf(output, ") {\n");
                generate_code(node->next, output);
                fprintf(output, "}\n\n");
            }
            break;
            
        case AST_BLOCK:
            generate_code(node->children, output);
            break;
            
        case AST_VARIABLE_DECLARATION:
            if (node->name) {
                fprintf(output, "    int %s;\n", node->name);
            }
            break;
            
        case AST_ASSIGNMENT:
            if (node->children && node->children->name) {
                fprintf(output, "    %s = ", node->children->name);
                generate_code(node->children->next, output);
                fprintf(output, ";\n");
            }
            break;
            
        case AST_IF_STATEMENT:
            fprintf(output, "    if (");
            generate_code(node->children, output);
            fprintf(output, ") {\n");
            generate_code(node->next, output);
            fprintf(output, "    }\n");
            break;
            
        case AST_IF_ELSE_STATEMENT:
            fprintf(output, "    if (");
            generate_code(node->children, output);
            fprintf(output, ") {\n");
            generate_code(node->next, output);
            fprintf(output, "    } else {\n");
            generate_code(node->right, output);
            fprintf(output, "    }\n");
            break;
            
        case AST_WHILE_STATEMENT:
            fprintf(output, "    while (");
            generate_code(node->children, output);
            fprintf(output, ") {\n");
            generate_code(node->next, output);
            fprintf(output, "    }\n");
            break;
            
        case AST_FOR_STATEMENT:
            fprintf(output, "    for (");
            generate_code(node->children, output);
            fprintf(output, "; ");
            generate_code(node->next, output);
            fprintf(output, "; ");
            generate_code(node->right, output);
            fprintf(output, ") {\n");
            generate_code(node->right->next, output);
            fprintf(output, "    }\n");
            break;
            
        case AST_RETURN:
            if (node->children) {
                fprintf(output, "    return ");
                generate_code(node->children, output);
                fprintf(output, ";\n");
            } else {
                fprintf(output, "    return;\n");
            }
            break;
            
        case AST_FUNCTION_CALL:
            if (node->name) {
                fprintf(output, "%s(", node->name);
                ASTNode* arg = node->children;
                int first = 1;
                while (arg) {
                    if (!first) fprintf(output, ", ");
                    generate_code(arg, output);
                    arg = arg->next;
                    first = 0;
                }
                fprintf(output, ")");
            }
            break;
            
        case AST_IDENTIFIER:
            if (node->name) {
                fprintf(output, "%s", node->name);
            }
            break;
            
        case AST_INTEGER:
            fprintf(output, "%d", node->value);
            break;
            
        case AST_FLOATING:
            fprintf(output, "%.3f", node->value / 1000.0);
            break;
            
        case AST_CHARACTER:
            fprintf(output, "'%c'", node->value);
            break;
            
        case AST_STRING:
            if (node->name) {
                fprintf(output, "\"%s\"", node->name);
            }
            break;
            
        case AST_ADD:
            generate_code(node->children, output);
            fprintf(output, " + ");
            generate_code(node->children->next, output);
            break;
            
        case AST_SUBTRACT:
            generate_code(node->children, output);
            fprintf(output, " - ");
            generate_code(node->children->next, output);
            break;
            
        case AST_MULTIPLY:
            generate_code(node->children, output);
            fprintf(output, " * ");
            generate_code(node->children->next, output);
            break;
            
        case AST_DIVIDE:
            generate_code(node->children, output);
            fprintf(output, " / ");
            generate_code(node->children->next, output);
            break;
            
        case AST_MODULO:
            generate_code(node->children, output);
            fprintf(output, " %% ");
            generate_code(node->children->next, output);
            break;
            
        case AST_EQUAL:
            generate_code(node->children, output);
            fprintf(output, " == ");
            generate_code(node->children->next, output);
            break;
            
        case AST_NOT_EQUAL:
            generate_code(node->children, output);
            fprintf(output, " != ");
            generate_code(node->children->next, output);
            break;
            
        case AST_LESS:
            generate_code(node->children, output);
            fprintf(output, " < ");
            generate_code(node->children->next, output);
            break;
            
        case AST_LESS_EQUAL:
            generate_code(node->children, output);
            fprintf(output, " <= ");
            generate_code(node->children->next, output);
            break;
            
        case AST_GREATER:
            generate_code(node->children, output);
            fprintf(output, " > ");
            generate_code(node->children->next, output);
            break;
            
        case AST_GREATER_EQUAL:
            generate_code(node->children, output);
            fprintf(output, " >= ");
            generate_code(node->children->next, output);
            break;
            
        case AST_LOGICAL_AND:
            generate_code(node->children, output);
            fprintf(output, " && ");
            generate_code(node->children->next, output);
            break;
            
        case AST_LOGICAL_OR:
            generate_code(node->children, output);
            fprintf(output, " || ");
            generate_code(node->children->next, output);
            break;
            
        case AST_LOGICAL_NOT:
            fprintf(output, "!");
            generate_code(node->children, output);
            break;
            
        case AST_UNARY_MINUS:
            fprintf(output, "-");
            generate_code(node->children, output);
            break;
            
        case AST_STATEMENT_LIST:
            generate_code(node->children, output);
            if (node->next) {
                generate_code(node->next, output);
            }
            break;
            
        default:
            break;
    }
}

void free_ast(ASTNode* node) {
    if (!node) return;
    
    if (node->name) {
        free(node->name);
    }
    
    if (node->children) {
        free_ast(node->children);
    }
    if (node->next) {
        free_ast(node->next);
    }
    if (node->right) {
        free_ast(node->right);
    }
    
    free(node);
}
