#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AST_PROGRAM,
    AST_DECLARATION,
    AST_VARIABLE_DECL,
    AST_FUNCTION_DECL,
    AST_TENSOR_DECL,
    AST_ASSIGNMENT,
    AST_BINARY_OP,
    AST_UNARY_OP,
    AST_IDENTIFIER,
    AST_NUMBER,
    AST_FLOAT_NUMBER,
    AST_STRING,
    AST_COMPOUND_STMT,
    AST_IF_STMT,
    AST_WHILE_STMT,
    AST_FOR_STMT,
    AST_RETURN_STMT,
    AST_EXPR_STMT,
    AST_FUNCTION_CALL,
    AST_TENSOR_ACCESS,
    AST_TENSOR_ADD,
    AST_TENSOR_SUB,
    AST_TENSOR_MUL,
    AST_PARAM_LIST,
    AST_ARG_LIST,
    AST_DECLARATION_LIST,
    AST_STATEMENT_LIST
} ASTNodeType;

typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_EQ,
    OP_NEQ,
    OP_LT,
    OP_GT,
    OP_LEQ,
    OP_GEQ,
    OP_AND,
    OP_OR,
    OP_NOT,
    OP_NEG,
    OP_ASSIGN
} OperatorType;

typedef struct ASTNode {
    ASTNodeType type;
    
    
    struct ASTNode *left;
    struct ASTNode *right;
    OperatorType op;
    
    
    struct ASTNode **children;
    int num_children;
    int children_capacity;
    
    
    char name[64];
    int int_value;
    double float_value;
    char string_value[256];
    
    
    char data_type[32];
    
    
    int is_tensor;
    int dims;
    int shape[10];
    
    
    struct ASTNode *condition;
    struct ASTNode *then_branch;
    struct ASTNode *else_branch;
    struct ASTNode *init;
    struct ASTNode *update;
    struct ASTNode *body;
    
    
    int line_number;
} ASTNode;


ASTNode* create_node(ASTNodeType type);
ASTNode* create_binary_node(ASTNodeType type, OperatorType op, ASTNode *left, ASTNode *right);
ASTNode* create_unary_node(ASTNodeType type, OperatorType op, ASTNode *operand);
ASTNode* create_identifier_node(const char *name);
ASTNode* create_number_node(int value);
ASTNode* create_float_node(double value);
ASTNode* create_string_node(const char *value);
ASTNode* create_tensor_decl_node(const char *name, int *shape, int dims);
ASTNode* create_tensor_op_node(ASTNodeType type, ASTNode *left, ASTNode *right);
ASTNode* create_assignment_node(ASTNode *lhs, ASTNode *rhs);
ASTNode* create_if_node(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch);
ASTNode* create_while_node(ASTNode *condition, ASTNode *body);
ASTNode* create_for_node(ASTNode *init, ASTNode *condition, ASTNode *update, ASTNode *body);
ASTNode* create_return_node(ASTNode *expr);
ASTNode* create_function_call_node(const char *name, ASTNode *args);
ASTNode* create_variable_decl_node(const char *type, const char *name, ASTNode *init);
ASTNode* create_function_decl_node(const char *return_type, const char *name, 
                                    ASTNode *params, ASTNode *body);


void add_child(ASTNode *parent, ASTNode *child);
ASTNode* create_list_node(ASTNodeType type);


void print_ast(ASTNode *node, int indent);
void print_ast_tree(ASTNode *node);


void free_ast(ASTNode *node);

#ifdef __cplusplus
}
#endif

#endif 
