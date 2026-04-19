# Mini C Compiler Demo

## Features

### Core Compilation Pipeline
- Full AST generation from source code
- Semantic analysis (type checking, scope validation)
- IR generation for tensor operations
- x86-64 assembly code generation with System V AMD64 ABI

### Advanced Compiler Features

#### 1. Control Flow Graph (CFG)
- **Basic block identification**: Identifies leaders and splits code into basic blocks
- **Edge construction**: Sequential fall-through, conditional branches, unconditional jumps
- **Dominator tree**: Cooper-Harvey-Kennedy algorithm for computing immediate dominators
- **Loop detection**: Back edge identification for natural loop analysis
- **RPO traversal**: Reverse post-order for efficient dataflow analysis

#### 2. AST to DAG Conversion
- **Hash-based deduplication**: FNV-1a hashing with 4096-entry hash table
- **Value numbering**: Canonical keys for operator/operand combinations
- **Constant folding**: Bottom-up evaluation of constant expressions (binary & unary ops)
- **Dead node elimination**: Removes nodes with zero in-degree (unreachable subtrees)
- **Reference counting**: Tracks parent-child relationships in the DAG

#### 3. Heap Allocation Runtime
- **Bump pointer allocator**: Fast 8-byte aligned allocation over 64KB static heap
- **Free list allocator**: First-fit search with 24-byte block headers
- **Block coalescing**: Merges adjacent free blocks on deallocation
- **IR lowering**: Transforms `alloc()` calls to runtime `__bump_alloc` or `__freelist_alloc`
- **Memory management**: `free()` support with automatic coalescing

#### 4. Stack Frame Management
- **System V AMD64 ABI**: Full compliance with calling convention
- **Prologue/epilogue**: Automatic generation with proper alignment
- **Callee-saved registers**: %rbx, %r12, %r13, %r14, %r15 preservation
- **Argument passing**: First 6 args in registers (rdi, rsi, rdx, rcx, r8, r9)
- **16-byte alignment**: Stack pointer aligned per ABI requirements

#### 5. Register Allocation
- **Liveness analysis**: Backward dataflow with use/def sets per basic block
- **Interference graph**: Tracks which variables are live simultaneously
- **Chaitin-Briggs coloring**: Graph coloring with K=13 physical registers
- **Register coalescing**: Eliminates copy instructions using Briggs criterion
- **Intelligent spilling**: Spills uncolorable variables to stack with load/store generation

### IR Optimizations
- **Constant folding**: Evaluates constant expressions at compile time
- **Common subexpression elimination (CSE)**: Reuses previously computed results
- **Dead code elimination**: Removes unused variable assignments

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

## Advanced Feature Testing

### Test All Features
Run comprehensive test that exercises all advanced compiler features:
```powershell
./mini_compiler test/test_advanced_features.c output.c advanced.s
type advanced.s
```

This test demonstrates:
- **CFG**: Multiple basic blocks with branches and loops
- **DAG**: Common subexpressions that get deduplicated
- **Heap allocation**: Dynamic memory allocation with `alloc()` and `free()`
- **Stack frames**: Function calls with proper ABI compliance
- **Register allocation**: Multiple live variables requiring intelligent allocation

### Viewing Compiler Internals

The compiler outputs detailed analysis to stdout:

#### Control Flow Graph
```
╔══════════════════════════════════════════════════════════════╗
║               Control Flow Graph  (N blocks)                 ║
╚══════════════════════════════════════════════════════════════╝
```
Shows basic blocks, predecessors, successors, and back edges.

#### Liveness Analysis
```
╔══════════════════════════════════════════════════════════════╗
║                    Liveness Analysis                         ║
╚══════════════════════════════════════════════════════════════╝
```
Shows which variables are live at entry/exit of each block.

#### Interference Graph
```
╔══════════════════════════════════════════════════════════════╗
║                  Interference Graph                          ║
╚══════════════════════════════════════════════════════════════╝
```
Shows which variables interfere (are live simultaneously).

#### Register Allocation Results
```
╔══════════════════════════════════════════════════════════════╗
║              Register Allocation (Chaitin-Briggs)            ║
╚══════════════════════════════════════════════════════════════╝
```
Shows physical register assignments and spilled variables.

### DAG Visualization
To see DAG deduplication and constant folding:
```powershell
./ast_ir_demo
```
Look for statistics on:
- Deduplication count (shared subexpressions)
- Fold count (constant expressions evaluated)
- Dead node count (unreachable nodes removed)

## Notes
- Symbol table + semantic errors print to stdout
- `output_tensor*.c` shows generated loop nests
- Assembly uses AT&T syntax for x86-64
- Assembly files show:
  - `.text` section for code
  - Heap runtime functions (`__bump_alloc`, `__freelist_alloc`, `__freelist_free`)
  - Function prologues with callee-saved register pushes
  - Function epilogues with register restoration
  - Register usage (physical regs: `%rbx`, `%r12-r15`, `%rdi-r9`)
  - Spill code (`movq` to/from stack slots)
  - Stack operations (`push`, `pop`, `mov`)
  - Arithmetic instructions (`add`, `sub`, `imul`, `idiv`)
  - Control flow (`jmp`, `je`, `jne`, `jl`, `jg`, etc.)
- CFG, liveness, interference graph, and register allocation results print to stdout
- Tool versions: `flex --version`, `bison --version`, etc.
