# Tensor Support in Mini C Compiler

## Overview

The Mini C Compiler now supports **tensor** operations with automatic code generation for element-wise operations.

---

## Features Implemented

### 1. Tensor Type Declaration

Declare tensors with multi-dimensional shapes:

```c
tensor A[2][3];
tensor B[2][3];
tensor C[2][3];
```

### 2. Symbol Table Integration

Tensors are stored in the symbol table with:
- **Name**: Identifier
- **Type**: `tensor`
- **Kind**: `TENSOR`
- **Dimensions**: Number of dimensions (e.g., 2 for `[2][3]`)
- **Shape**: Array of dimension sizes (e.g., `[2, 3]`)

Example symbol table output:
```
Name           Type      Kind        Line    Scope   Shape
---------------------------------------------------------------------
A              tensor    tensor      6       0       [2][3]
B              tensor    tensor      7       0       [2][3]
C              tensor    tensor      8       0       [2][3]
```

### 3. Supported Operations

Element-wise operations:
- **Addition**: `C = A + B`
- **Subtraction**: `C = A - B`
- **Multiplication**: `C = A * B` (element-wise, not matrix multiplication)

### 4. Semantic Checks

The compiler performs the following checks:
- Both operands must be tensors
- Tensors must have the same number of dimensions
- Tensors must have matching shapes

**Error example:**
```c
tensor A[2][3];
tensor B[3][2];
C = A + B;  // Error: tensor shape mismatch
```

Output:
```
Semantic Error (line 11): tensor shape mismatch: A and B have incompatible shapes
```

### 5. Code Generation

Tensor operations are expanded into nested loops:

**Input:**
```c
tensor A[2][3];
tensor B[2][3];
tensor C[2][3];

int main() {
    C = A + B;
    return 0;
}
```

**Generated Code** (saved to output file):
```c
for(int i0=0; i0<2; i0++) {
    for(int i1=0; i1<3; i1++) {
        C[i0][i1] = A[i0][i1] + B[i0][i1];
    }
}
```

---

## Usage

### Compile with Code Generation

```bash
./mini_compiler <input.c> <output.c>
```

Example:
```bash
./mini_compiler test_tensor_valid.c generated_code.c
```

### Compile without Code Generation

```bash
./mini_compiler <input.c>
```

---

## Test Cases

### Valid Programs

#### Example 1: Basic Addition
```c
tensor A[2][2];
tensor B[2][2];
tensor C[2][2];

int main() {
    C = A + B;
    return 0;
}
```

#### Example 2: Multiplication
```c
tensor X[3][3];
tensor Y[3][3];
tensor Z[3][3];

int main() {
    Z = X * Y;
    return 0;
}
```

#### Example 3: Multiple Operations
```c
tensor A[2][3];
tensor B[2][3];
tensor C[2][3];
tensor D[2][3];

int main() {
    C = A + B;
    D = A - B;
    return 0;
}
```

### Invalid Programs

#### Shape Mismatch
```c
tensor A[2][3];
tensor B[3][2];
tensor C[2][3];

int main() {
    C = A + B;  // Error: incompatible shapes
    return 0;
}
```

---

## Implementation Details

### Lexer Changes
- Added `TOKEN_TENSOR` keyword recognition in `lexer.l`

### Parser Changes
- Added `tensor_declaration` grammar rule
- Added `dimension_list` to parse `[2][3]` syntax
- Extended `assignment_expression` to handle tensor operations
- Added semantic checking functions:
  - `check_tensor_compatibility()`: Validates shape matching
  - `generate_tensor_operation()`: Generates nested loop code

### Symbol Table Changes
- Extended `SymbolEntry` with:
  - `bool is_tensor`
  - `int num_dimensions`
  - `std::vector<int> shape`
- Added `insert_tensor()` method
- Updated `print()` to display tensor shapes

---

## Limitations

The current implementation:
- ✅ Supports element-wise operations (+, -, *)
- ✅ Supports multi-dimensional tensors
- ✅ Performs shape checking
- ✅ Generates loop-based code
- ❌ Does NOT support matrix multiplication
- ❌ Does NOT support broadcasting
- ❌ Does NOT support dynamic shapes
- ❌ Does NOT support tensor indexing in expressions

---

## Running Tests

```bash
# Build the compiler
make clean
make

# Test valid tensor programs
./mini_compiler test_tensor_valid.c output1.c
./mini_compiler test_tensor_valid2.c output2.c
./mini_compiler test_tensor_valid3.c output3.c

# Test invalid tensor program (should show error)
./mini_compiler test_tensor_invalid.c
```

---

## Example Session

```bash
$ ./mini_compiler test_tensor_valid.c output.c
Parsing 'test_tensor_valid.c'...

Generated code written to 'output.c'

===== Symbol Table =====
Name           Type      Kind        Line    Scope   Shape
---------------------------------------------------------------------
A              tensor    tensor      6       0       [2][2]
B              tensor    tensor      7       0       [2][2]
C              tensor    tensor      8       0       [2][2]
main           int       function    13      0
========================

Parsing completed successfully. No errors found.

$ cat output.c
for(int i0=0; i0<2; i0++) {
    for(int i1=0; i1<2; i1++) {
        C[i0][i1] = A[i0][i1] + B[i0][i1];
    }
}
```
