#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "ir.h"
#include "ir_gen.h"
#include "symbol_table.h"

SymbolTable sym_table;

ASTNode* build_tensor_example_ast() {
    ASTNode *program = create_list_node(AST_PROGRAM);
    int shape[] = {2, 2};
    add_child(program, create_tensor_decl_node("A", shape, 2));
    add_child(program, create_tensor_decl_node("B", shape, 2));
    add_child(program, create_tensor_decl_node("C", shape, 2));
    ASTNode *main_body = create_list_node(AST_STATEMENT_LIST);
    
    // C = A + B
    ASTNode *tensor_add = create_tensor_op_node(AST_TENSOR_ADD,
                                                 create_identifier_node("A"),
                                                 create_identifier_node("B"));
    ASTNode *assignment = create_assignment_node(create_identifier_node("C"), tensor_add);
    add_child(main_body, assignment);
    add_child(main_body, create_return_node(create_number_node(0)));
    
    ASTNode *main_func = create_function_decl_node("int", "main", NULL, main_body);
    add_child(program, main_func);
    
    return program;
}

ASTNode* build_tensor_mul_ast() {
    ASTNode *program = create_list_node(AST_PROGRAM);
    
    int shape[] = {3, 3};
    add_child(program, create_tensor_decl_node("X", shape, 2));
    add_child(program, create_tensor_decl_node("Y", shape, 2));
    add_child(program, create_tensor_decl_node("Z", shape, 2));
    
    ASTNode *main_body = create_list_node(AST_STATEMENT_LIST);
    
    ASTNode *tensor_mul = create_tensor_op_node(AST_TENSOR_MUL,
                                                 create_identifier_node("X"),
                                                 create_identifier_node("Y"));
    ASTNode *assignment = create_assignment_node(create_identifier_node("Z"), tensor_mul);
    add_child(main_body, assignment);
    
    add_child(main_body, create_return_node(create_number_node(0)));
    
    ASTNode *main_func = create_function_decl_node("int", "main", NULL, main_body);
    add_child(program, main_func);
    
    return program;
}

ASTNode* build_tensor_multi_ast() {
    ASTNode *program = create_list_node(AST_PROGRAM);
    
    int shape[] = {2, 3};
    add_child(program, create_tensor_decl_node("A", shape, 2));
    add_child(program, create_tensor_decl_node("B", shape, 2));
    add_child(program, create_tensor_decl_node("C", shape, 2));
    add_child(program, create_tensor_decl_node("D", shape, 2));
    
    ASTNode *main_body = create_list_node(AST_STATEMENT_LIST);
    
    // C = A + B
    ASTNode *tensor_add = create_tensor_op_node(AST_TENSOR_ADD,
                                                 create_identifier_node("A"),
                                                 create_identifier_node("B"));
    add_child(main_body, create_assignment_node(create_identifier_node("C"), tensor_add));
    
    // D = A - B
    ASTNode *tensor_sub = create_tensor_op_node(AST_TENSOR_SUB,
                                                 create_identifier_node("A"),
                                                 create_identifier_node("B"));
    add_child(main_body, create_assignment_node(create_identifier_node("D"), tensor_sub));
    
    add_child(main_body, create_return_node(create_number_node(0)));
    
    ASTNode *main_func = create_function_decl_node("int", "main", NULL, main_body);
    add_child(program, main_func);
    
    return program;
}

void setup_symbol_table_for_demo(int *shape, int dims) {
    // Manually insert tensor symbols for demo
    std::vector<int> shape_vec(shape, shape + dims);
    sym_table.insert_tensor("A", shape_vec, 1);
    sym_table.insert_tensor("B", shape_vec, 1);
    sym_table.insert_tensor("C", shape_vec, 1);
}

void demo_example_1() {
    printf("\n========================================\n");
    printf("Example 1: C = A + B (2x2 tensors)\n");
    printf("========================================\n");
    
    int shape[] = {2, 2};
    setup_symbol_table_for_demo(shape, 2);
    
    ASTNode *ast = build_tensor_example_ast();
    print_ast_tree(ast);
    
    IRList *ir = generate_ir_from_ast(ast, &sym_table);
    print_ir_list(ir);
    
    free_ast(ast);
    free_ir_list(ir);
}

void demo_example_2() {
    printf("\n========================================\n");
    printf("Example 2: Z = X * Y (3x3 tensors)\n");
    printf("========================================\n");
    
    // Reset symbol table
    while (sym_table.current_scope() > 0) {
        sym_table.exit_scope();
    }
    
    int shape[] = {3, 3};
    std::vector<int> shape_vec(shape, shape + 2);
    sym_table.insert_tensor("X", shape_vec, 1);
    sym_table.insert_tensor("Y", shape_vec, 1);
    sym_table.insert_tensor("Z", shape_vec, 1);
    
    ASTNode *ast = build_tensor_mul_ast();
    print_ast_tree(ast);
    
    IRList *ir = generate_ir_from_ast(ast, &sym_table);
    print_ir_list(ir);
    
    free_ast(ast);
    free_ir_list(ir);
}

void demo_example_3() {
    printf("\n========================================\n");
    printf("Example 3: Multiple operations (2x3 tensors)\n");
    printf("========================================\n");
    
    // Reset symbol table
    while (sym_table.current_scope() > 0) {
        sym_table.exit_scope();
    }
    
    int shape[] = {2, 3};
    std::vector<int> shape_vec(shape, shape + 2);
    sym_table.insert_tensor("A", shape_vec, 1);
    sym_table.insert_tensor("B", shape_vec, 1);
    sym_table.insert_tensor("C", shape_vec, 1);
    sym_table.insert_tensor("D", shape_vec, 1);
    
    ASTNode *ast = build_tensor_multi_ast();
    print_ast_tree(ast);
    
    IRList *ir = generate_ir_from_ast(ast, &sym_table);
    print_ir_list(ir);
    
    free_ast(ast);
    free_ir_list(ir);
}

int main(int argc, char *argv[]) {
    printf("===========================================\n");
    printf("AST and IR Generation Demo\n");
    printf("===========================================\n");
    
    demo_example_1();
    demo_example_2();
    demo_example_3();
    
    printf("\n===========================================\n");
    printf("Demo completed successfully!\n");
    printf("===========================================\n");
    
    return 0;
}
