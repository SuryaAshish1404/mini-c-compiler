# ============================================================
# Mini C Compiler - Makefile
# ============================================================
# Toolchain
CXX       = g++
CC        = gcc
CXXFLAGS  = -std=c++17 -Wall -Wextra -g -Isrc
CFLAGS    = -std=c11 -Wall -Wextra -g -Isrc
FLEX      = flex
BISON     = bison

# Directories
SRC_DIR   = src
TEST_DIR  = test

# Output binary
TARGET    = mini_compiler

# Generated sources
BISON_SRC = parser.tab.cpp
BISON_HDR = parser.tab.hpp
FLEX_SRC  = lex.yy.cpp

# Project sources
SRCS      = $(SRC_DIR)/symbol_table.cpp $(SRC_DIR)/ast.c $(SRC_DIR)/ir.c $(SRC_DIR)/temp_var.c $(SRC_DIR)/tensor_ir.c $(SRC_DIR)/ir_gen.cpp $(SRC_DIR)/semantic.cpp $(SRC_DIR)/codegen.c $(SRC_DIR)/optimizer.c
LEXER_BIN = lexer_tokens
LEXER_DRV = $(SRC_DIR)/lexer_driver.cpp
DEMO_BIN  = ast_ir_demo
DEMO_SRC  = $(SRC_DIR)/ast_ir_demo.c

# ============================================================
# Build targets
# ============================================================

all: $(TARGET) $(LEXER_BIN) $(DEMO_BIN)

# Generate parser (Bison)
$(BISON_SRC) $(BISON_HDR): $(SRC_DIR)/parser.y
	$(BISON) -d -o $(BISON_SRC) $(SRC_DIR)/parser.y

# Generate lexer (Flex)
$(FLEX_SRC): $(SRC_DIR)/lexer.l $(BISON_HDR)
	$(FLEX) -o $(FLEX_SRC) $(SRC_DIR)/lexer.l

# Compile C sources
ast.o: $(SRC_DIR)/ast.c $(SRC_DIR)/ast.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/ast.c

ir.o: $(SRC_DIR)/ir.c $(SRC_DIR)/ir.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/ir.c

temp_var.o: $(SRC_DIR)/temp_var.c $(SRC_DIR)/temp_var.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/temp_var.c

tensor_ir.o: $(SRC_DIR)/tensor_ir.c $(SRC_DIR)/tensor_ir.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/tensor_ir.c

ir_gen.o: $(SRC_DIR)/ir_gen.cpp $(SRC_DIR)/ir_gen.h
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/ir_gen.cpp

semantic.o: $(SRC_DIR)/semantic.cpp $(SRC_DIR)/semantic.h
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/semantic.cpp

codegen.o: $(SRC_DIR)/codegen.c $(SRC_DIR)/codegen.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/codegen.c

optimizer.o: $(SRC_DIR)/optimizer.c $(SRC_DIR)/optimizer.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/optimizer.c

cfg.o: $(SRC_DIR)/cfg.c $(SRC_DIR)/cfg.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/cfg.c

dag.o: $(SRC_DIR)/dag.c $(SRC_DIR)/dag.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/dag.c

heap_alloc.o: $(SRC_DIR)/heap_alloc.c $(SRC_DIR)/heap_alloc.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/heap_alloc.c

stack_frame.o: $(SRC_DIR)/stack_frame.c $(SRC_DIR)/stack_frame.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/stack_frame.c

regalloc.o: $(SRC_DIR)/regalloc.c $(SRC_DIR)/regalloc.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/regalloc.c

OBJS = ast.o ir.o temp_var.o tensor_ir.o ir_gen.o semantic.o codegen.o \
       optimizer.o cfg.o dag.o heap_alloc.o stack_frame.o regalloc.o

# Link everything
$(TARGET): $(FLEX_SRC) $(BISON_SRC) $(SRC_DIR)/symbol_table.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(BISON_SRC) $(FLEX_SRC) $(SRC_DIR)/symbol_table.cpp $(OBJS)

$(LEXER_BIN): $(FLEX_SRC) $(BISON_HDR) $(LEXER_DRV)
	$(CXX) $(CXXFLAGS) -o $(LEXER_BIN) $(LEXER_DRV) $(FLEX_SRC)

$(DEMO_BIN): $(DEMO_SRC) $(OBJS) $(SRC_DIR)/symbol_table.cpp
	$(CXX) $(CXXFLAGS) -o $(DEMO_BIN) $(DEMO_SRC) $(OBJS) $(SRC_DIR)/symbol_table.cpp

# ============================================================
# Utility targets
# ============================================================

# Run against the sample test file
test: $(TARGET) $(LEXER_BIN)
	./$(TARGET) $(TEST_DIR)/test.c
	./$(LEXER_BIN) $(TEST_DIR)/test.c

clean:
	rm -f $(TARGET) $(LEXER_BIN) $(DEMO_BIN) $(BISON_SRC) $(BISON_HDR) $(FLEX_SRC) parser.output *.o

.PHONY: all clean test
