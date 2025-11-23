# TACO Compiler Makefile
# Compiles all source files and creates the TACO compiler

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./include
LDFLAGS = 

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

# Source and header files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
HEADERS = $(wildcard $(INC_DIR)/*.h)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Executable file name
TARGET = taco

# Main target - building the compiler
all: $(TARGET)

# Linking all object files into the compiler
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "TACO compiler has been built: ./$(TARGET)"

# Compiling .cpp files into .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Creating the directory for object files
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Cleaning object and executable files
clean:
	rm -rf $(OBJ_DIR) $(TARGET)
	@echo "Build files have been cleaned"

# Rebuilding from scratch
rebuild: clean all

# Displaying file information
info:
	@echo "Source files: $(SOURCES)"
	@echo "Header files: $(HEADERS)"
	@echo "Object files: $(OBJECTS)"
	@echo "Target: $(TARGET)"

.PHONY: all clean rebuild info
