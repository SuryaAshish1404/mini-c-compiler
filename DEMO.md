# Mini C Compiler — Demo Commands

> Copy-paste these commands into PowerShell to build, run, and demo the project.

---

## 1. Set PATH (run once per PowerShell session)

```powershell
$env:Path = "C:\msys64\usr\bin;C:\msys64\mingw64\bin;" + $env:Path
```

## 2. Clean & Build

```powershell
make clean
```

```powershell
make
```

## 3. Run — Valid Syntax

```powershell
./mini_compiler test/test.c
```

**Expected output:**
```
Parsing 'test.c'...

===== Symbol Table =====
Name           Type      Kind        Line    Scope
-----------------------------------------------------
main           int       function    71      0
b              int       parameter   17      0
a              int       parameter   17      0
pi             float     variable    14      0
add            int       function    19      0
globalCount    int       variable    13      0
========================

Parsing completed successfully. No errors found.
```

## 4. Run — Invalid Syntax (Error Reporting)

```powershell
./mini_compiler test/test_invalid.c
```

**Expected output:**
```
Parsing 'test_invalid.c'...

Syntax Error (line 9): syntax error near 'int'

===== Symbol Table =====
Name           Type      Kind        Line    Scope
-----------------------------------------------------
========================

Parsing finished with 1 error(s).
```

## 5. Run — Tensor Operations (Valid)

```powershell
./mini_compiler test/test_tensor_valid.c output_tensor1.c
```

**Expected output:**
```
Parsing 'test_tensor_valid.c'...

Generated code written to 'output_tensor1.c'

===== Symbol Table =====
Name           Type      Kind        Line    Scope   Shape
---------------------------------------------------------------------
A              tensor    tensor      6       0       [2][2]
B              tensor    tensor      7       0       [2][2]
C              tensor    tensor      8       0       [2][2]
main           int       function    13      0
========================

Parsing completed successfully. No errors found.
```

## 6. Run — Tensor Operations (Invalid Shape Mismatch)

```powershell
./mini_compiler test/test_tensor_invalid.c
```

**Expected output:**
```
Parsing 'test_tensor_invalid.c'...

Semantic Error (line 11): tensor shape mismatch: A and B have incompatible shapes

===== Symbol Table =====
Name           Type      Kind        Line    Scope   Shape
---------------------------------------------------------------------
A              tensor    tensor      6       0       [2][3]
B              tensor    tensor      7       0       [3][2]
C              tensor    tensor      8       0       [2][3]
main           int       function    13      0
========================

Parsing finished with 1 error(s).
```

## 7. View Generated Tensor Code

```powershell
cat output_tensor1.c
```

**Expected output:**
```c
for(int i0=0; i0<2; i0++) {
    for(int i1=0; i1<2; i1++) {
        C[i0][i1] = A[i0][i1] + B[i0][i1];
    }
}
```

## 8. Run AST/IR Demo (full tree + IR printout)

```powershell
./ast_ir_demo
```

**What you should see:**
- Three scenarios (2x2 add, 3x3 mul, mixed ops)
- Each prints the AST tree followed by the IR listing
- Tensor loops appear as nested `FOR` instructions with `LOAD/STORE`

## 9. Scripted AST/IR Test (PowerShell helper)

```powershell
./test_ast_ir.ps1
```

**Purpose:** Sets PATH, rebuilds everything, and runs the AST/IR demo in one step.

## 10. Additional References

- `AST_IR_DOCUMENTATION.md` — architecture write-up with diagrams/examples
- `output_tensor*.c` — generated tensor C loops from compiler runs
- `ast_ir_demo.c` — source for the demo scenarios (helpful when presenting)

## 11. Verify Toolchain (optional)

```powershell
flex --version
bison --version
g++ --version
make --version
```
