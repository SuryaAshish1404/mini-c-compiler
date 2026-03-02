# ============================================================
# Mini C Compiler - Makefile
# ============================================================
# Toolchain
CXX       = g++
CC        = gcc
CXXFLAGS  = -std=c++17 -Wall -Wextra -g
CFLAGS    = -std=c11 -Wall -Wextra -g
FLEX      = flex
BISON     = bison

# Output binary
TARGET    = mini_compiler

# Generated sources
BISON_SRC = parser.tab.cpp
BISON_HDR = parser.tab.hpp
FLEX_SRC  = lex.yy.cpp

# Project sources
SRCS      = symbol_table.cpp ast.c ir.c temp_var.c tensor_ir.c ir_gen.c
LEXER_BIN = lexer_tokens
LEXER_DRV = lexer_driver.cpp
DEMO_BIN  = ast_ir_demo
DEMO_SRC  = ast_ir_demo.c

# ============================================================
# Build targets
# ============================================================

all: $(TARGET) $(LEXER_BIN) $(DEMO_BIN)

# Generate parser (Bison)
$(BISON_SRC) $(BISON_HDR): parser.y
	$(BISON) -d -o $(BISON_SRC) parser.y

# Generate lexer (Flex)
$(FLEX_SRC): lexer.l $(BISON_HDR)
	$(FLEX) -o $(FLEX_SRC) lexer.l

# Compile C sources
ast.o: ast.c ast.h
	$(CC) $(CFLAGS) -c ast.c

ir.o: ir.c ir.h
	$(CC) $(CFLAGS) -c ir.c

temp_var.o: temp_var.c temp_var.h
	$(CC) $(CFLAGS) -c temp_var.c

tensor_ir.o: tensor_ir.c tensor_ir.h
	$(CC) $(CFLAGS) -c tensor_ir.c

ir_gen.o: ir_gen.cpp ir_gen.h
	$(CXX) $(CXXFLAGS) -c ir_gen.cpp

# Link everything
$(TARGET): $(FLEX_SRC) $(BISON_SRC) symbol_table.cpp ast.o ir.o temp_var.o tensor_ir.o ir_gen.o
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(BISON_SRC) $(FLEX_SRC) symbol_table.cpp ast.o ir.o temp_var.o tensor_ir.o ir_gen.o

$(LEXER_BIN): $(FLEX_SRC) $(BISON_HDR) $(LEXER_DRV)
	$(CXX) $(CXXFLAGS) -o $(LEXER_BIN) $(LEXER_DRV) $(FLEX_SRC)

$(DEMO_BIN): $(DEMO_SRC) ast.o ir.o temp_var.o tensor_ir.o ir_gen.o symbol_table.cpp
	$(CXX) $(CXXFLAGS) -o $(DEMO_BIN) $(DEMO_SRC) ast.o ir.o temp_var.o tensor_ir.o ir_gen.o symbol_table.cpp

# ============================================================
# Utility targets
# ============================================================

# Run against the sample test file
test: $(TARGET) $(LEXER_BIN)
	./$(TARGET) test.c
	./$(LEXER_BIN) test.c

clean:
	rm -f $(TARGET) $(LEXER_BIN) $(DEMO_BIN) $(BISON_SRC) $(BISON_HDR) $(FLEX_SRC) parser.output *.o

.PHONY: all clean test
