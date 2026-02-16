# ============================================================
# Mini C Compiler - Makefile
# ============================================================
# Toolchain
CXX       = g++
CXXFLAGS  = -std=c++17 -Wall -Wextra -g
FLEX      = flex
BISON     = bison

# Output binary
TARGET    = mini_compiler

# Generated sources
BISON_SRC = parser.tab.cpp
BISON_HDR = parser.tab.hpp
FLEX_SRC  = lex.yy.cpp

# Project sources
SRCS      = symbol_table.cpp
LEXER_BIN = lexer_tokens
LEXER_DRV = lexer_driver.cpp

# ============================================================
# Build targets
# ============================================================

all: $(TARGET) $(LEXER_BIN)

# Generate parser (Bison)
$(BISON_SRC) $(BISON_HDR): parser.y
	$(BISON) -d -o $(BISON_SRC) parser.y

# Generate lexer (Flex)
$(FLEX_SRC): lexer.l $(BISON_HDR)
	$(FLEX) -o $(FLEX_SRC) lexer.l

# Link everything
$(TARGET): $(FLEX_SRC) $(BISON_SRC) $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(BISON_SRC) $(FLEX_SRC) $(SRCS)

$(LEXER_BIN): $(FLEX_SRC) $(BISON_HDR) $(LEXER_DRV)
	$(CXX) $(CXXFLAGS) -o $(LEXER_BIN) $(LEXER_DRV) $(FLEX_SRC)

# ============================================================
# Utility targets
# ============================================================

# Run against the sample test file
test: $(TARGET) $(LEXER_BIN)
	./$(TARGET) test.c
	./$(LEXER_BIN) test.c

clean:
	rm -f $(TARGET) $(LEXER_BIN) $(BISON_SRC) $(BISON_HDR) $(FLEX_SRC) parser.output

.PHONY: all clean test
