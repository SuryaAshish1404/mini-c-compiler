#include "ast.h"

ASTNode* create_node(ASTNodeType type) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Error: failed to allocate AST node\n");
        exit(1);
    }
    
    memset(node, 0, sizeof(ASTNode));
    node->type = type;
    node->children = NULL;
    node->num_children = 0;
    node->children_capacity = 0;
    node->is_tensor = 0;
    node->dims = 0;
    
    return node;
}

ASTNode* create_binary_node(ASTNodeType type, OperatorType op, ASTNode *left, ASTNode *right) {
    ASTNode *node = create_node(type);
    node->op = op;
    node->left = left;
    node->right = right;
    return node;
}

ASTNode* create_unary_node(ASTNodeType type, OperatorType op, ASTNode *operand) {
    ASTNode *node = create_node(type);
    node->op = op;
    node->left = operand;
    return node;
}

ASTNode* create_identifier_node(const char *name) {
    ASTNode *node = create_node(AST_IDENTIFIER);
    strncpy(node->name, name, sizeof(node->name) - 1);
    return node;
}

ASTNode* create_number_node(int value) {
    ASTNode *node = create_node(AST_NUMBER);
    node->int_value = value;
    return node;
}

ASTNode* create_float_node(double value) {
    ASTNode *node = create_node(AST_FLOAT_NUMBER);
    node->float_value = value;
    return node;
}

ASTNode* create_string_node(const char *value) {
    ASTNode *node = create_node(AST_STRING);
    strncpy(node->string_value, value, sizeof(node->string_value) - 1);
    return node; 
}

ASTNode* create_tensor_decl_node(const char *name, int *shape, int dims) {
    ASTNode *node = create_node(AST_TENSOR_DECL);
    strncpy(node->name, name, sizeof(node->name) - 1);
    node->is_tensor = 1;
    node->dims = dims;
    for (int i = 0; i < dims && i < 10; i++) {
        node->shape[i] = shape[i];
    }
    return node;
}

ASTNode* create_tensor_op_node(ASTNodeType type, ASTNode *left, ASTNode *right) {
    ASTNode *node = create_node(type);
    node->left = left;
    node->right = right;
    return node;
}

ASTNode* create_assignment_node(ASTNode *lhs, ASTNode *rhs) {
    ASTNode *node = create_node(AST_ASSIGNMENT);
    node->left = lhs;
    node->right = rhs;
    return node;
}

ASTNode* create_if_node(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch) {
    ASTNode *node = create_node(AST_IF_STMT);
    node->condition = condition;
    node->then_branch = then_branch;
    node->else_branch = else_branch;
    return node;
}

ASTNode* create_while_node(ASTNode *condition, ASTNode *body) {
    ASTNode *node = create_node(AST_WHILE_STMT);
    node->condition = condition;
    node->body = body;
    return node;
}

ASTNode* create_for_node(ASTNode *init, ASTNode *condition, ASTNode *update, ASTNode *body) {
    ASTNode *node = create_node(AST_FOR_STMT);
    node->init = init;
    node->condition = condition;
    node->update = update;
    node->body = body;
    return node;
}

ASTNode* create_return_node(ASTNode *expr) {
    ASTNode *node = create_node(AST_RETURN_STMT);
    node->left = expr;
    return node;
}

ASTNode* create_function_call_node(const char *name, ASTNode *args) {
    ASTNode *node = create_node(AST_FUNCTION_CALL);
    strncpy(node->name, name, sizeof(node->name) - 1);
    node->left = args;
    return node;
}

ASTNode* create_variable_decl_node(const char *type, const char *name, ASTNode *init) {
    ASTNode *node = create_node(AST_VARIABLE_DECL);
    strncpy(node->data_type, type, sizeof(node->data_type) - 1);
    strncpy(node->name, name, sizeof(node->name) - 1);
    node->right = init;
    return node;
}

ASTNode* create_function_decl_node(const char *return_type, const char *name, 
                                    ASTNode *params, ASTNode *body) {
    ASTNode *node = create_node(AST_FUNCTION_DECL);
    strncpy(node->data_type, return_type, sizeof(node->data_type) - 1);
    strncpy(node->name, name, sizeof(node->name) - 1);
    node->left = params;
    node->body = body;
    return node;
}

ASTNode* create_list_node(ASTNodeType type) {
    ASTNode *node = create_node(type);
    node->children_capacity = 4;
    node->children = (ASTNode**)malloc(sizeof(ASTNode*) * node->children_capacity);
    node->num_children = 0;
    return node;
}

void add_child(ASTNode *parent, ASTNode *child) {
    if (!parent || !child) return;
    
    if (parent->children == NULL) {
        parent->children_capacity = 4;
        parent->children = (ASTNode**)malloc(sizeof(ASTNode*) * parent->children_capacity);
        parent->num_children = 0;
    }
    
    if (parent->num_children >= parent->children_capacity) {
        parent->children_capacity *= 2;
        parent->children = (ASTNode**)realloc(parent->children, 
                                              sizeof(ASTNode*) * parent->children_capacity);
    }
    
    parent->children[parent->num_children++] = child;
}

static const char* op_to_string(OperatorType op) {
    switch (op) {
        case OP_ADD: return "+";
        case OP_SUB: return "-";
        case OP_MUL: return "*";
        case OP_DIV: return "/";
        case OP_MOD: return "%";
        case OP_EQ: return "==";
        case OP_NEQ: return "!=";
        case OP_LT: return "<";
        case OP_GT: return ">";
        case OP_LEQ: return "<=";
        case OP_GEQ: return ">=";
        case OP_AND: return "&&";
        case OP_OR: return "||";
        case OP_NOT: return "!";
        case OP_NEG: return "-";
        case OP_ASSIGN: return "=";
        default: return "?";
    }
}

static const char* node_type_to_string(ASTNodeType type) {
    switch (type) {
        case AST_PROGRAM: return "Program";
        case AST_DECLARATION: return "Declaration";
        case AST_VARIABLE_DECL: return "VariableDecl";
        case AST_FUNCTION_DECL: return "FunctionDecl";
        case AST_TENSOR_DECL: return "TensorDecl";
        case AST_ASSIGNMENT: return "Assignment";
        case AST_BINARY_OP: return "BinaryOp";
        case AST_UNARY_OP: return "UnaryOp";
        case AST_IDENTIFIER: return "Identifier";
        case AST_NUMBER: return "Number";
        case AST_FLOAT_NUMBER: return "Float";
        case AST_STRING: return "String";
        case AST_COMPOUND_STMT: return "CompoundStmt";
        case AST_IF_STMT: return "IfStmt";
        case AST_WHILE_STMT: return "WhileStmt";
        case AST_FOR_STMT: return "ForStmt";
        case AST_RETURN_STMT: return "ReturnStmt";
        case AST_EXPR_STMT: return "ExprStmt";
        case AST_FUNCTION_CALL: return "FunctionCall";
        case AST_TENSOR_ACCESS: return "TensorAccess";
        case AST_TENSOR_ADD: return "TensorAdd";
        case AST_TENSOR_SUB: return "TensorSub";
        case AST_TENSOR_MUL: return "TensorMul";
        case AST_PARAM_LIST: return "ParamList";
        case AST_ARG_LIST: return "ArgList";
        case AST_DECLARATION_LIST: return "DeclList";
        case AST_STATEMENT_LIST: return "StmtList";
        default: return "Unknown";
    }
}

void print_ast(ASTNode *node, int indent) {
    if (!node) return;
    
    for (int i = 0; i < indent; i++) printf("  ");
    
    printf("%s", node_type_to_string(node->type));
    
    switch (node->type) {
        case AST_IDENTIFIER:
        case AST_VARIABLE_DECL:
        case AST_FUNCTION_DECL:
        case AST_FUNCTION_CALL:
        case AST_TENSOR_DECL:
            printf(" '%s'", node->name);
            if (node->type == AST_TENSOR_DECL) {
                printf(" [");
                for (int i = 0; i < node->dims; i++) {
                    if (i > 0) printf("][");
                    printf("%d", node->shape[i]);
                }
                printf("]");
            }
            if (node->data_type[0]) {
                printf(" : %s", node->data_type);
            }
            break;
        case AST_NUMBER:
            printf(" %d", node->int_value);
            break;
        case AST_FLOAT_NUMBER:
            printf(" %f", node->float_value);
            break;
        case AST_STRING:
            printf(" \"%s\"", node->string_value);
            break;
        case AST_BINARY_OP:
        case AST_UNARY_OP:
            printf(" %s", op_to_string(node->op));
            break;
        default:
            break;
    }
    
    printf("\n");
    
    if (node->left) {
        print_ast(node->left, indent + 1);
    }
    if (node->right) {
        print_ast(node->right, indent + 1);
    }
    if (node->condition) {
        for (int i = 0; i < indent + 1; i++) printf("  ");
        printf("Condition:\n");
        print_ast(node->condition, indent + 2);
    }
    if (node->then_branch) {
        for (int i = 0; i < indent + 1; i++) printf("  ");
        printf("Then:\n");
        print_ast(node->then_branch, indent + 2);
    }
    if (node->else_branch) {
        for (int i = 0; i < indent + 1; i++) printf("  ");
        printf("Else:\n");
        print_ast(node->else_branch, indent + 2);
    }
    if (node->init) {
        for (int i = 0; i < indent + 1; i++) printf("  ");
        printf("Init:\n");
        print_ast(node->init, indent + 2);
    }
    if (node->update) {
        for (int i = 0; i < indent + 1; i++) printf("  ");
        printf("Update:\n");
        print_ast(node->update, indent + 2);
    }
    if (node->body) {
        for (int i = 0; i < indent + 1; i++) printf("  ");
        printf("Body:\n");
        print_ast(node->body, indent + 2);
    }
    if (node->children) {
        for (int i = 0; i < node->num_children; i++) {
            print_ast(node->children[i], indent + 1);
        }
    }
}

void print_ast_tree(ASTNode *node) {
    printf("\n===== Abstract Syntax Tree =====\n");
    print_ast(node, 0);
    printf("================================\n\n");
}

void free_ast(ASTNode *node) {
    if (!node) return;
    
    free_ast(node->left);
    free_ast(node->right);
    free_ast(node->condition);
    free_ast(node->then_branch);
    free_ast(node->else_branch);
    free_ast(node->init);
    free_ast(node->update);
    free_ast(node->body);
    
    if (node->children) {
        for (int i = 0; i < node->num_children; i++) {
            free_ast(node->children[i]);
        }
        free(node->children);
    }
    
    free(node);
}
