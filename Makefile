CXX = g++
CXXFLAGS = -std=c++14 -Wall -I./include
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = .
OBJ_DIR = obj

# Targets
LEXER = $(BUILD_DIR)/lex
PARSER = $(BUILD_DIR)/parser
TACCO = $(BUILD_DIR)/tacco

# Source files
LEXER_SRC = $(SRC_DIR)/lexer.cpp
PARSER_SRC = $(SRC_DIR)/parser.cpp

# Object files
LEXER_OBJ = $(OBJ_DIR)/lexer.o
PARSER_OBJ = $(OBJ_DIR)/parser.o
LEXER_MAIN_OBJ = $(OBJ_DIR)/lexer_main.o

# Default target
all: $(LEXER) $(PARSER)

# Build complete taco compiler (combines lexer and parser)
tacco: $(TACCO)

$(TACCO): $(PARSER_SRC) $(LEXER_OBJ) $(INCLUDE_DIR)/parser.h $(INCLUDE_DIR)/lexer.h
	$(CXX) $(CXXFLAGS) -o $(TACCO) $(PARSER_SRC) $(LEXER_OBJ)

# Create obj directory
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Build lexer executable
$(LEXER): $(LEXER_SRC) $(INCLUDE_DIR)/lexer.h
	$(CXX) $(CXXFLAGS) -o $(LEXER) $(LEXER_SRC)

# Build lexer object file (without main) for linking with parser
$(LEXER_OBJ): $(LEXER_SRC) $(INCLUDE_DIR)/lexer.h | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -DLIB_BUILD -o $(LEXER_OBJ) $(LEXER_SRC)

# Build parser (needs to link with lexer code)
$(PARSER): $(PARSER_SRC) $(INCLUDE_DIR)/parser.h $(INCLUDE_DIR)/lexer.h $(LEXER_OBJ)
	$(CXX) $(CXXFLAGS) -o $(PARSER) $(PARSER_SRC) $(LEXER_OBJ)

# Clean build artifacts
clean:
	rm -f $(LEXER) $(PARSER) $(TACCO)
	rm -rf $(OBJ_DIR)


.PHONY: all clean test tacco
