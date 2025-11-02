CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -I./include
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = .
OBJ_DIR = obj

# Main compiler target
TACO = $(BUILD_DIR)/taco

# Source files
LEXER_SRC = $(SRC_DIR)/lexer.cpp
PARSER_SRC = $(SRC_DIR)/parser.cpp
TAC_SRC = $(SRC_DIR)/tac_gen.cpp
ENERGY_SRC = $(SRC_DIR)/energy.cpp
CODEGEN_SRC = $(SRC_DIR)/codegen.cpp
MAIN_SRC = $(SRC_DIR)/main.cpp

# Object files
LEXER_OBJ = $(OBJ_DIR)/lexer.o
PARSER_OBJ = $(OBJ_DIR)/parser.o
TAC_OBJ = $(OBJ_DIR)/tac_gen.o
ENERGY_OBJ = $(OBJ_DIR)/energy.o
CODEGEN_OBJ = $(OBJ_DIR)/codegen.o
MAIN_OBJ = $(OBJ_DIR)/main.o

# All object files
ALL_OBJS = $(LEXER_OBJ) $(PARSER_OBJ) $(TAC_OBJ) $(ENERGY_OBJ) $(CODEGEN_OBJ) $(MAIN_OBJ)

# Default target
all: $(TACO)

# Create obj directory
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Build main TACO compiler
$(TACO): $(ALL_OBJS)
	$(CXX) $(CXXFLAGS) -o $(TACO) $(ALL_OBJS)
	@echo "TACO compiler built successfully!"

# Compile individual object files
$(LEXER_OBJ): $(LEXER_SRC) $(INCLUDE_DIR)/lexer.h | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $(LEXER_OBJ) $(LEXER_SRC)

$(PARSER_OBJ): $(PARSER_SRC) $(INCLUDE_DIR)/parser.h $(INCLUDE_DIR)/lexer.h | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $(PARSER_OBJ) $(PARSER_SRC)

$(TAC_OBJ): $(TAC_SRC) $(INCLUDE_DIR)/tac.h $(INCLUDE_DIR)/parser.h | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $(TAC_OBJ) $(TAC_SRC)

$(ENERGY_OBJ): $(ENERGY_SRC) $(INCLUDE_DIR)/energy.h $(INCLUDE_DIR)/tac.h | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $(ENERGY_OBJ) $(ENERGY_SRC)

$(CODEGEN_OBJ): $(CODEGEN_SRC) $(INCLUDE_DIR)/codegen.h $(INCLUDE_DIR)/tac.h | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $(CODEGEN_OBJ) $(CODEGEN_SRC)

$(MAIN_OBJ): $(MAIN_SRC) $(INCLUDE_DIR)/lexer.h $(INCLUDE_DIR)/parser.h $(INCLUDE_DIR)/tac.h $(INCLUDE_DIR)/energy.h $(INCLUDE_DIR)/codegen.h | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $(MAIN_OBJ) $(MAIN_SRC)

# Test targets
test: $(TACO)
	@echo "Running simple test..."
	./$(TACO) --tokens --ast --tac --energy examples/simple.taco -o test_output.c
	@echo "\nCompiling generated C code..."
	gcc test_output.c -o test_output -lm
	@echo "Running generated program..."
	./test_output
	@echo "\nTest completed!"

test-fft: $(TACO)
	@echo "Compiling FFT example..."
	./$(TACO) --tac --energy examples/fft.taco -o fft_output.c
	gcc fft_output.c -o fft_output -lm
	./fft_output

test-dct: $(TACO)
	@echo "Compiling DCT example..."
	./$(TACO) --tac --energy examples/dct.taco -o dct_output.c
	gcc dct_output.c -o dct_output -lm
	./dct_output

# Run all tests
test-all: test test-fft test-dct
	@echo "\nAll tests completed!"

# Clean build artifacts
clean:
	rm -f $(TACO)
	rm -rf $(OBJ_DIR)
	rm -f *_output.c *_output
	rm -f output.c

# Show help
help:
	@echo "TACO Compiler Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all        - Build the TACO compiler (default)"
	@echo "  test       - Run simple test"
	@echo "  test-fft   - Run FFT test"
	@echo "  test-dct   - Run DCT test"
	@echo "  test-all   - Run all tests"
	@echo "  clean      - Remove all build artifacts"
	@echo "  help       - Show this help message"

.PHONY: all test test-fft test-dct test-all clean help

