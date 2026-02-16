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
./mini_compiler test.c
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
./mini_compiler test_invalid.c
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

## 5. Verify Toolchain (optional)

```powershell
flex --version
bison --version
g++ --version
make --version
```
