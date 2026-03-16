# Mini C Compiler demo

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

## Notes
- Symbol table + IR text will print to stdout for each run.
- `output_tensor*.c` shows the generated loop nests.
- Tool versions if needed: `flex --version`, `bison --version`, etc.
