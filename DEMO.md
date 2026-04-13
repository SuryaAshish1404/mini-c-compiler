# Mini C Compiler demo

## Features
- Full AST generation from source code
- Semantic analysis (type checking, scope validation)
- IR generation for tensor operations
- IR optimizations:
  - Constant folding
  - Common subexpression elimination (CSE)
  - Dead code elimination
- x86-64 assembly code generation

## Setup
```
$env:Path = "C:\msys64\usr\bin;C:\msys64\mingw64\bin;" + $env:Path
make clean
make
```

## Quick sanity checks
```
./mini_compiler test/test.c
./mini_compiler test/test_invalid.c
./mini_compiler test/test3.c   # expected failure, shows parser error
```

## Tensor runs
```
./mini_compiler test/test_tensor_valid.c output_tensor1.c
./mini_compiler test/test_tensor_valid2.c output_tensor2.c
./mini_compiler test/test_tensor_valid3.c output_tensor3.c
./mini_compiler test/test_tensor_invalid.c   # currently passes (TODO: add failing sample)
type output_tensor1.c
```

## AST/IR demo + helper script
```
./ast_ir_demo
./test_ast_ir.ps1
```

## Semantic Analysis & Assembly Generation
```
./mini_compiler test/test_simple.c [tensor_output.c] [assembly_output.s]
```
- First arg: source file (required)
- Second arg: tensor C code output (optional)
- Third arg: assembly output (optional)

The compiler now performs:
1. Parsing → AST generation
2. Semantic analysis (type checking, function validation)
3. IR generation
4. IR optimizations:
   - **Constant folding**: Evaluates constant expressions at compile time (e.g., `5 + 3` → `8`)
   - **Common subexpression elimination (CSE)**: Reuses previously computed results (e.g., `a + b` computed once)
   - **Dead code elimination**: Removes unused variable assignments
5. Assembly code generation (x86-64)

## Quick Test Script

Run all optimization tests at once:
```
.\run_optimization_tests.ps1
```

## Optimization Test Cases

### Test 1: Constant Folding
Demonstrates compile-time evaluation of constant expressions.
```
./mini_compiler test/test_constant_folding.c
```
Expected: `10 + 5` → `15`, `20 * 3` → `60`, etc. computed at compile time.

### Test 2: Common Subexpression Elimination (CSE)
Shows reuse of previously computed results.
```
./mini_compiler test/test_cse_demo.c
```
Expected: Multiple `a + b` computations replaced with single computation + reuse.

### Test 3: Dead Code Elimination
Removes unused variable assignments.
```
./mini_compiler test/test_dead_code.c
```
Expected: `unused1`, `unused2`, and `temp` assignments eliminated.

### Test 4: All Optimizations Combined
Demonstrates all three optimizations working together.
```
./mini_compiler test/test_all_optimizations.c
```
Expected:
- Constant folding: `5 + 3` → `8`, `10 * 2` → `20`, `7 - 2` → `5`
- CSE: Duplicate `a + b` expressions eliminated
- Dead code: `unused` assignment removed

## Assembly Code Generation Test Cases

### Basic Assembly Generation
Generates x86-64 assembly with functions and control flow.
```
./mini_compiler test/test_assembly_gen.c output.c test_assembly.s
type test_assembly.s
```
Expected output:
- Function prologues/epilogues
- Arithmetic operations in assembly
- Function calls with parameters
- Conditional jumps for if-statements
- Return value handling

### Simple Program with Assembly Output
```
./mini_compiler test/test_simple.c output.c simple.s
type simple.s
```

### Optimized Code with Assembly
See optimizations reflected in generated assembly.
```
./mini_compiler test/test_all_optimizations.c output.c optimized.s
type optimized.s
```
Expected: Cleaner assembly due to constant folding and dead code elimination.

## Understanding Optimization Output

When you run the compiler, look for:
```
Optimizations applied: X changes
```
This tells you how many optimizations were performed.

## Notes
- Symbol table + semantic errors print to stdout
- `output_tensor*.c` shows generated loop nests
- Assembly uses AT&T syntax for x86-64
- Assembly files show:
  - `.text` section for code
  - Function labels
  - Register usage (`%rax`, `%rbx`, etc.)
  - Stack operations (`push`, `pop`, `mov`)
  - Arithmetic instructions (`add`, `sub`, `imul`, `idiv`)
  - Control flow (`jmp`, `je`, `jne`, `jl`, `jg`, etc.)
- Tool versions: `flex --version`, `bison --version`, etc.
