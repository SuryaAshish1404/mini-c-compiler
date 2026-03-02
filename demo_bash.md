# Mini C Compiler — Demo Commands (Bash)

> Copy/paste these commands into a Unix-like shell (Linux/macOS) to build, run, and demo the project.

---

## 1. Set PATH (run once per shell session)

```bash
export PATH="/mingw64/bin:/usr/local/bin:$PATH"
```
*(Adjust the toolchain paths above if needed for your environment.)*

## 2. Clean & Build

```bash
make clean
make
```

## 3. Run — Valid Syntax

```bash
./mini_compiler test.c
```

**Expected output:**
```
Parsing 'test.c'...

===== Symbol Table =====
... (standard symbol table listing)

Parsing completed successfully. No errors found.
```

## 4. Run — Invalid Syntax (Error Reporting)

```bash
./mini_compiler test_invalid.c
```

**Expected output:**
```
Parsing 'test_invalid.c'...
Syntax Error (line 9): syntax error near 'int'
...
Parsing finished with 1 error(s).
```

## 5. Run — Tensor Operations (Valid)

```bash
./mini_compiler test_tensor_valid.c output_tensor1.c
```

Key lines:
```
Generated code written to 'output_tensor1.c'
Parsing completed successfully. No errors found.
```

## 6. Run — Tensor Operations (Invalid Shape Mismatch)

```bash
./mini_compiler test_tensor_invalid.c
```

Expected message:
```
Semantic Error (line 11): tensor shape mismatch: A and B have incompatible shapes
Parsing finished with 1 error(s).
```

## 7. View Generated Tensor Code

```bash
cat output_tensor1.c
```

Printed snippet should resemble nested loops performing `C = A + B`.

## 8. Run AST/IR Demo (full tree + IR printout)

```bash
./ast_ir_demo
```

You should see:
- Three scenarios (2x2 add, 3x3 mul, mixed ops)
- Each scenario prints the AST tree followed by the IR listing
- Tensor loops show up as nested `FOR` instructions with `LOAD/STORE`

## 9. Scripted AST/IR Test (Bash helper)

```bash
bash ./test_ast_ir.ps1
```
*(If you prefer bash, run PowerShell script via `pwsh ./test_ast_ir.ps1` on systems with PowerShell Core.)*

## 10. Additional References

- `AST_IR_DOCUMENTATION.md` — architecture write-up with diagrams/examples
- `output_tensor*.c` — generated tensor C loops from compiler runs
- `ast_ir_demo.c` — source for the demo scenarios (helpful when presenting)

## 11. Verify Toolchain (optional)

```bash
flex --version
bison --version
g++ --version
make --version
```
