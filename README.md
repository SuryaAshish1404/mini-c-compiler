# Mini C Compiler

A lexical analyzer and syntax analyzer for a simplified C-like language, built with **Flex**, **Bison**, and **C++**.

---

## Table of Contents

1. [Overview](#overview)
2. [Prerequisites](#prerequisites)
3. [Building](#building)
4. [Usage](#usage)
5. [Token Specifications](#token-specifications)
6. [Grammar Rules (CFG)](#grammar-rules-cfg)
7. [Symbol Table](#symbol-table)
8. [Error Handling](#error-handling)
9. [Project Structure](#project-structure)

---

## Overview

This project implements the front-end of a compiler for a minimal C-like language with **tensor support**:

- **Lexical Analysis** (`lexer.l`) — Tokenizes source code using Flex.
- **Syntax Analysis** (`parser.y`) — Parses token streams against a context-free grammar using Bison.
- **Symbol Table** (`symbol_table.h/cpp`) — Stores identifiers with type, scope, and declaration info.
- **Tensor Operations** — Supports multi-dimensional tensor declarations and element-wise operations (+, -, *) with automatic code generation.

---

## Prerequisites

| Tool    | Minimum Version |
|---------|-----------------|
| g++     | 7+ (C++17)      |
| Flex    | 2.6+            |
| Bison   | 3.0+            |
| Make    | 3.81+           |

**Windows users:** Install these via [MSYS2](https://www.msys2.org/) or [WSL](https://learn.microsoft.com/en-us/windows/wsl/).

```bash
# MSYS2 example
pacman -S mingw-w64-x86_64-gcc flex bison make
```

---

## Building

```bash
# Build the compiler + lexer driver
make

# Clean generated files
make clean
```

This produces two executables:

- `mini_compiler` — parses a program and prints the symbol table.
- `lexer_tokens` — runs only the lexer and prints every token with lexemes.

---

## Usage

```bash
# Parse a source file
./mini_compiler <source_file.c>

# Parse and generate code for tensor operations
./mini_compiler <source_file.c> <output_file.c>

# Tokenize only (lexer demo)
./lexer_tokens <source_file.c>

# Run parser + lexer on the default sample
make test
```

**Example output (valid program):**

```
Parsing 'test.c'...

===== Symbol Table =====
Name           Type      Kind        Line    Scope
-----------------------------------------------------
globalCount    int       variable    13      0
pi             float     variable    14      0
...
========================

Parsing completed successfully. No errors found.
```

**Example output (lexer only):**

```
Tokenizing 'test.c'...
Line  | Token                  | Lexeme
------------------------------------------------------------
1     | TOKEN_INT              | int
1     | TOKEN_IDENTIFIER       | globalCount
1     | TOKEN_SEMICOLON        | ;
...
------------------------------------------------------------
Lexical analysis completed.
```

**Example output (invalid program):**

```
Syntax Error (line 9): syntax error near 'int'
Parsing finished with 1 error(s).
```

---

## Token Specifications

### Keywords

| Token          | Lexeme   |
|----------------|----------|
| `TOKEN_INT`    | `int`    |
| `TOKEN_FLOAT`  | `float`  |
| `TOKEN_CHAR`   | `char`   |
| `TOKEN_VOID`   | `void`   |
| `TOKEN_TENSOR` | `tensor` |
| `TOKEN_IF`     | `if`     |
| `TOKEN_ELSE`   | `else`   |
| `TOKEN_WHILE`  | `while`  |
| `TOKEN_FOR`    | `for`    |
| `TOKEN_RETURN` | `return` |

### Operators

| Token                | Lexeme | Description            |
|----------------------|--------|------------------------|
| `TOKEN_PLUS`         | `+`    | Addition               |
| `TOKEN_MINUS`        | `-`    | Subtraction / Negation |
| `TOKEN_STAR`         | `*`    | Multiplication         |
| `TOKEN_SLASH`        | `/`    | Division               |
| `TOKEN_PERCENT`      | `%`    | Modulo                 |
| `TOKEN_ASSIGN`       | `=`    | Assignment             |
| `TOKEN_EQ`           | `==`   | Equality               |
| `TOKEN_NEQ`          | `!=`   | Not equal              |
| `TOKEN_LT`           | `<`    | Less than              |
| `TOKEN_GT`           | `>`    | Greater than           |
| `TOKEN_LEQ`          | `<=`   | Less or equal          |
| `TOKEN_GEQ`          | `>=`   | Greater or equal       |
| `TOKEN_AND`          | `&&`   | Logical AND            |
| `TOKEN_OR`           | `\|\|` | Logical OR             |
| `TOKEN_NOT`          | `!`    | Logical NOT            |
| `TOKEN_INCREMENT`    | `++`   | Increment              |
| `TOKEN_DECREMENT`    | `--`   | Decrement              |
| `TOKEN_PLUS_ASSIGN`  | `+=`   | Add-assign             |
| `TOKEN_MINUS_ASSIGN` | `-=`   | Subtract-assign        |
| `TOKEN_STAR_ASSIGN`  | `*=`   | Multiply-assign        |
| `TOKEN_SLASH_ASSIGN` | `/=`   | Divide-assign          |

### Literals

| Token                 | Pattern                  | Example     |
|-----------------------|--------------------------|-------------|
| `TOKEN_INT_LITERAL`   | `[0-9]+`                 | `42`        |
| `TOKEN_FLOAT_LITERAL` | `[0-9]+\.[0-9]*`         | `3.14`      |
| `TOKEN_STRING_LITERAL`| `"([^"\\]|\\.)*"`        | `"hello"`   |

### Identifiers

| Token              | Pattern               | Example     |
|--------------------|-----------------------|-------------|
| `TOKEN_IDENTIFIER` | `[a-zA-Z_][a-zA-Z0-9_]*` | `myVar` |

### Delimiters

| Token             | Lexeme |
|-------------------|--------|
| `TOKEN_LPAREN`    | `(`    |
| `TOKEN_RPAREN`    | `)`    |
| `TOKEN_LBRACE`    | `{`    |
| `TOKEN_RBRACE`    | `}`    |
| `TOKEN_LBRACKET`  | `[`    |
| `TOKEN_RBRACKET`  | `]`    |
| `TOKEN_SEMICOLON` | `;`    |
| `TOKEN_COMMA`     | `,`    |

### Comments

| Style        | Syntax           |
|--------------|------------------|
| Single-line  | `// ...`         |
| Multi-line   | `/* ... */`      |

---

## Grammar Rules (CFG)

The parser implements the following context-free grammar with proper operator precedence and associativity.

### Precedence Table (lowest to highest)

| Precedence | Operators                          | Associativity |
|------------|------------------------------------|---------------|
| 1          | `= += -= *= /=`                   | Right         |
| 2          | `\|\|`                             | Left          |
| 3          | `&&`                               | Left          |
| 4          | `== !=`                            | Left          |
| 5          | `< > <= >=`                        | Left          |
| 6          | `+ -`                              | Left          |
| 7          | `* / %`                            | Left          |
| 8          | `! -` (unary)                      | Right         |
| 9          | `++ --` (postfix)                  | Left          |

### Production Rules (simplified)

```
program             → declaration_list
declaration_list    → declaration_list declaration | declaration
declaration         → variable_declaration | function_declaration

type_specifier      → int | float | char | void

variable_declaration → type_specifier IDENTIFIER ;
                     | type_specifier IDENTIFIER = expression ;

function_declaration → type_specifier IDENTIFIER ( parameter_list ) compound_statement
                     | type_specifier IDENTIFIER ( ) compound_statement

compound_statement  → { statement_list } | { }
statement           → expression_statement | variable_declaration
                     | compound_statement | selection_statement
                     | iteration_statement | return_statement

selection_statement → if ( expression ) statement
                    | if ( expression ) statement else statement

iteration_statement → while ( expression ) statement
                    | for ( expr_stmt expr_stmt expression ) statement

expression          → assignment | logical_or
logical_or          → logical_or || logical_and | logical_and
logical_and         → logical_and && equality | equality
equality            → equality == relational | equality != relational | relational
relational          → relational < additive | ... | additive
additive            → additive + multiplicative | additive - multiplicative | multiplicative
multiplicative      → multiplicative * unary | multiplicative / unary | unary
unary               → - unary | ! unary | ++ IDENTIFIER | -- IDENTIFIER | postfix
postfix             → primary | IDENTIFIER ++ | IDENTIFIER -- | IDENTIFIER ( args )
primary             → INT_LITERAL | FLOAT_LITERAL | STRING_LITERAL | IDENTIFIER | ( expression )
```

---

## Symbol Table

The symbol table (`symbol_table.h/cpp`) uses a **scope stack** of hash maps:

- **Insert** — Adds a symbol to the current scope; reports an error on duplicates.
- **Lookup** — Searches from the innermost scope outward (lexical scoping).
- **Scope management** — `enter_scope()` / `exit_scope()` push and pop scope levels.

Each entry stores:

| Field           | Description                        |
|-----------------|------------------------------------|
| `name`          | Identifier name                    |
| `type`          | Data type (`int`, `float`, etc.)   |
| `kind`          | `VARIABLE`, `FUNCTION`, `PARAMETER`|
| `line_declared` | Source line number                  |
| `scope_level`   | Nesting depth (0 = global)         |

---

## Error Handling

### Lexical Errors

Unrecognized characters are reported with their line number:

```
Lexical Error (line 15): unrecognized character '@'
```

### Syntax Errors

Parse errors include the line number and the offending token:

```
Syntax Error (line 7): syntax error near 'int'
```

---

## Project Structure

```
mini-c-compiler/
├── lexer.l                        # Flex lexical analyzer definitions
├── parser.y                       # Bison grammar rules
├── symbol_table.h / .cpp          # Symbol table implementation
├── lexer_driver.cpp               # Standalone lexer demo
├── Makefile                       # Build automation
├── test.c                         # Valid syntax showcase
├── test_invalid.c                 # Mixed invalid constructs
├── test_invalid_control.c         # Control-flow syntax errors
├── test_invalid_declarations.c    # Declaration errors
├── test_invalid_expressions.c     # Expression errors
├── test_invalid.c                 # Simple invalid example
├── DEMO.md                        # Copy/paste demo commands
└── README.md                      # This file
```

### Sample Input Files

- `test.c` – comprehensive valid program exercising declarations, control flow, functions, unary/binary operators, and scopes.
- `test_invalid.c` – compact invalid cases (missing braces, bad operators).
- `test_invalid_control.c` – malformed if/else/loops and missing braces.
- `test_invalid_declarations.c` – declaration/redeclaration and initializer issues.
- `test_invalid_expressions.c` – malformed expressions and operators.
