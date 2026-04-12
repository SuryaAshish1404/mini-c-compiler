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

## Notes
- Symbol table + semantic errors print to stdout
- `output_tensor*.c` shows generated loop nests
- Assembly uses AT&T syntax for x86-64
- Tool versions: `flex --version`, `bison --version`, etc.
