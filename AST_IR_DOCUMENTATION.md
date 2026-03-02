# AST and IR Implementation Documentation

## Overview

The Mini C Compiler now includes a complete **Abstract Syntax Tree (AST)** and **Intermediate Representation (IR)** system for advanced code analysis and generation.

---

## Architecture

### 1. AST (Abstract Syntax Tree)

The AST represents the syntactic structure of the source program as a tree.

#### AST Node Types

```c
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
    // ... more types
} ASTNodeType;
```

#### AST Node Structure

```c
typedef struct ASTNode {
    ASTNodeType type;
    
    // For binary/unary operations
    struct ASTNode *left;
    struct ASTNode *right;
    OperatorType op;
    
    // For multi-child nodes
    struct ASTNode **children;
    int num_children;
    
    // For identifiers and literals
    char name[64];
    int int_value;
    double float_value;
    
    // For tensor information
    int is_tensor;
    int dims;
    int shape[10];
    
    // For control flow
    struct ASTNode *condition;
    struct ASTNode *then_branch;
    struct ASTNode *else_branch;
    struct ASTNode *body;
    
    int line_number;
} ASTNode;
```

### 2. IR (Intermediate Representation)

The IR uses **three-address code** format for low-level operations.

#### IR Instruction Types

```c
typedef enum {
    IR_ASSIGN,      // x = y
    IR_ADD,         // x = y + z
    IR_SUB,         // x = y - z
    IR_MUL,         // x = y * z
    IR_DIV,         // x = y / z
    IR_LOAD,        // x = LOAD array[index]
    IR_STORE,       // STORE array[index] = value
    IR_LABEL,       // L1:
    IR_GOTO,        // GOTO L1
    IR_IF_FALSE,    // IF_FALSE condition GOTO L1
    IR_FOR_BEGIN,   // FOR i = 0 TO n
    IR_FOR_END,     // END_FOR
    IR_CALL,        // x = CALL func(args)
    IR_RETURN,      // RETURN x
    // ... more opcodes
} IROpcode;
```

#### IR Instruction Structure

```c
typedef struct IR {
    IROpcode opcode;
    char op[16];           // Operation string
    char arg1[64];         // First argument
    char arg2[64];         // Second argument
    char result[64];       // Result/destination
    int label_num;         // For labels and jumps
    struct IR *next;       // Linked list
} IR;
```

---

## Module Organization

### Core Modules

| Module | Files | Purpose |
|--------|-------|---------|
| **AST** | `ast.h`, `ast.c` | AST node creation, manipulation, printing |
| **IR** | `ir.h`, `ir.c` | IR instruction creation, management, printing |
| **IR Generation** | `ir_gen.h`, `ir_gen.cpp` | Traverse AST and generate IR |
| **Temp Variables** | `temp_var.h`, `temp_var.c` | Generate temporary variable names (t0, t1, ...) |
| **Tensor IR** | `tensor_ir.h`, `tensor_ir.c` | Specialized IR generation for tensor operations |

---

## Example: Tensor Addition

### Input Program

```c
tensor A[2][2];
tensor B[2][2];
tensor C[2][2];

int main() {
    C = A + B;
    return 0;
}
```

### Generated AST

```
Program
  ├── TensorDecl 'A' [2][2]
  ├── TensorDecl 'B' [2][2]
  ├── TensorDecl 'C' [2][2]
  └── FunctionDecl 'main' : int
        └── Body:
              └── StmtList
                    ├── Assignment
                    │     ├── Identifier 'C'
                    │     └── TensorAdd
                    │           ├── Identifier 'A'
                    │           └── Identifier 'B'
                    └── ReturnStmt
                          └── Number 0
```

### Generated IR

```
FOR i0 = 0 TO 2
    FOR i1 = 0 TO 2
        t0 = LOAD A[i0][i1]
        t1 = LOAD B[i0][i1]
        t2 = t0 + t1
        STORE C[i0][i1] = t2
    END_FOR
END_FOR
RETURN 0
```

---

## API Reference

### AST Creation Functions

```c
// Basic nodes
ASTNode* create_node(ASTNodeType type);
ASTNode* create_identifier_node(const char *name);
ASTNode* create_number_node(int value);

// Binary operations
ASTNode* create_binary_node(ASTNodeType type, OperatorType op, 
                            ASTNode *left, ASTNode *right);

// Tensor operations
ASTNode* create_tensor_decl_node(const char *name, int *shape, int dims);
ASTNode* create_tensor_op_node(ASTNodeType type, ASTNode *left, ASTNode *right);

// Control flow
ASTNode* create_if_node(ASTNode *condition, ASTNode *then_branch, 
                       ASTNode *else_branch);
ASTNode* create_while_node(ASTNode *condition, ASTNode *body);
ASTNode* create_for_node(ASTNode *init, ASTNode *condition, 
                        ASTNode *update, ASTNode *body);

// Lists
ASTNode* create_list_node(ASTNodeType type);
void add_child(ASTNode *parent, ASTNode *child);

// Printing and cleanup
void print_ast_tree(ASTNode *node);
void free_ast(ASTNode *node);
```

### IR Creation Functions

```c
// IR list management
IRList* create_ir_list();
void append_ir(IRList *list, IR *instruction);

// Basic instructions
IR* create_ir_assign(const char *result, const char *arg1);
IR* create_ir_binary(IROpcode opcode, const char *result, 
                     const char *arg1, const char *arg2);

// Memory operations
IR* create_ir_load(const char *result, const char *array, const char *index);
IR* create_ir_store(const char *array, const char *index, const char *value);

// Control flow
IR* create_ir_label(int label_num);
IR* create_ir_goto(int label_num);
IR* create_ir_if(IROpcode opcode, const char *condition, int label_num);

// Loops
IR* create_ir_for_begin(const char *var, const char *start, const char *end);
IR* create_ir_for_end();

// Printing
void print_ir_list(IRList *list);
```

### IR Generation

```c
// Main IR generation (C++ only)
IRList* generate_ir_from_ast(ASTNode *ast, SymbolTable *sym_table);

// Tensor-specific IR generation
void generate_tensor_loops(IRList *ir_list, TensorInfo *tensor_info, 
                          const char *dest, const char *lhs, 
                          const char *rhs, const char *op);
```

### Temporary Variable Generation

```c
void reset_temp_counter();
void get_temp_var(char *buffer, size_t size);  // Generates t0, t1, t2, ...
void get_label(char *buffer, size_t size);     // Generates L0, L1, L2, ...
void get_loop_var(char *buffer, size_t size, int level);  // Generates i0, i1, ...
```

---

## Usage

### Running the Demo

```bash
# Build all components
make clean
make

# Run AST/IR demo
./ast_ir_demo
```

### Demo Output

The demo program demonstrates:
1. **Example 1**: 2x2 tensor addition (`C = A + B`)
2. **Example 2**: 3x3 tensor multiplication (`Z = X * Y`)
3. **Example 3**: Multiple operations on 2x3 tensors

Each example shows:
- Complete AST tree structure
- Generated IR instructions
- Instruction count

---

## Tensor IR Generation Details

### Loop Expansion

Tensor operations are expanded into nested loops based on dimensions:

**For a 2D tensor `[2][3]`:**

```
FOR i0 = 0 TO 2
    FOR i1 = 0 TO 3
        // Operation here
    END_FOR
END_FOR
```

### Index Mapping

Tensors are stored as flat arrays with index mapping:

```
A[i][j] → A[i * cols + j]
```

For multi-dimensional tensors:
```
A[i][j][k] → A[i * (dim2 * dim3) + j * dim3 + k]
```

### Operation Pattern

Each tensor operation follows this pattern:

1. **FOR loops** for each dimension
2. **LOAD** left operand
3. **LOAD** right operand
4. **Perform operation** (ADD/SUB/MUL)
5. **STORE** result
6. **END_FOR** for each dimension

---

## Implementation Notes

### C/C++ Interoperability

The system uses both C and C++:
- **C modules**: AST, IR, temp_var, tensor_ir (for portability)
- **C++ modules**: ir_gen (requires SymbolTable)

All C headers include `extern "C"` guards for C++ compatibility.

### Memory Management

- AST nodes are dynamically allocated
- Use `free_ast()` to recursively free AST trees
- IR instructions are managed in linked lists
- Use `free_ir_list()` to free IR instruction lists

### Temporary Variables

- Counter-based generation: `t0`, `t1`, `t2`, ...
- Reset counter with `reset_temp_counter()` before each function
- Loop variables: `i0`, `i1`, `i2`, ... for tensor dimensions

---

## Testing

### Test Cases Included

1. **test_tensor_valid.c** - Basic 2x2 addition
2. **test_tensor_valid2.c** - 3x3 multiplication
3. **test_tensor_valid3.c** - Multiple operations

### Verification

Run the demo to verify:
```bash
./ast_ir_demo
```

Expected output includes:
- ✅ Properly formatted AST trees
- ✅ Correct IR instruction sequences
- ✅ Nested FOR loops for tensor operations
- ✅ Proper temporary variable usage

---

## Future Enhancements

Potential extensions:
- [ ] Code optimization passes on IR
- [ ] Register allocation
- [ ] Target code generation (x86, ARM, etc.)
- [ ] IR-level optimizations (constant folding, dead code elimination)
- [ ] Support for more complex tensor operations
- [ ] Type checking at IR level

---

## References

- **AST Module**: `ast.h`, `ast.c`
- **IR Module**: `ir.h`, `ir.c`
- **IR Generation**: `ir_gen.h`, `ir_gen.cpp`
- **Demo Program**: `ast_ir_demo.c`
- **Tensor Features**: `TENSOR_FEATURES.md`
