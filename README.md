# TACO Compiler

**Three-Address Code Compiler** - A compiler implementation that translates source code through lexical analysis, parsing, three-address code generation, and native code generation.

## Overview

TACO is a complete compiler toolchain written in C++ that implements the full compilation pipeline from source code to executable binaries. The compiler demonstrates fundamental compiler construction techniques including lexical analysis, syntax parsing, intermediate representation generation, and code generation.

## Features

- **Lexical Analysis**: Tokenization of source code with comprehensive token recognition
- **Syntax Parsing**: Recursive descent parser generating Abstract Syntax Trees (AST)
- **Three-Address Code (TAC)**: Intermediate representation for optimization and analysis
- **Code Generation**: Transpilation to C code with optional direct compilation to executable
- **Comprehensive Logging**: Detailed logging system for tokens, AST, TAC, and performance profiling
- **Control Flow Structures**: Support for if statements, while loops, and for loops
- **Arithmetic & Logical Operations**: Full expression evaluation with operator precedence
- **Type System**: Integer and floating-point number support

## Architecture

The compiler is organized into several distinct phases:

1. **Lexer** (`lexer.cpp`/`lexer.h`): Performs lexical analysis and tokenization
2. **Parser** (`parser.cpp`/`parser.h`): Builds Abstract Syntax Tree from tokens
3. **TAC Generator** (`tac_gen.cpp`/`tac.h`): Generates three-address code intermediate representation
4. **Code Generator** (`codegen.cpp`/`codegen.h`): Produces C code from TAC
5. **Logger** (`logger.cpp`/`logger.h`): Handles compilation logging and diagnostics

## Building

### Prerequisites

- C++17 compatible compiler (g++ recommended)
- Make build system
- GCC (for final executable generation when not using `--c-only`)

### Compilation

Build the TACO compiler:

```bash
make
```

Clean build artifacts:

```bash
make clean
```

Rebuild from scratch:

```bash
make rebuild
```

View build information:

```bash
make info
```

## Usage

### Basic Compilation

Compile a TACO source file to an executable:

```bash
./taco source_file.taco
```

### Command-Line Options

```
Usage: taco [options] <source_file.taco>

Options:
  -o <file>         Output executable file (default: output)
  --gen-c           Generate C code file (required for --c-only)
  --c-only          Generate only C code without compiling (implies --gen-c)
  --log <options>   Enable logging with specified components (comma-separated)
                    Options: tokens, ast, tac, timing, profile, all
                    Example: --log tokens,ast or --log all
                    Log file: compilation_DDMMYYYY_HHMMSS.log
  --help            Show this help message
```

### Examples

Generate executable named `program`:

```bash
./taco -o program source.taco
```

Generate C code without compiling:

```bash
./taco --c-only source.taco
```

Compile with full logging:

```bash
./taco --log all source.taco
```

Compile with selective logging:

```bash
./taco --log tokens,ast,tac source.taco
```

## Language Support

The TACO compiler supports a simple imperative programming language with the following constructs:

### Variable Declarations and Assignments

```
let x = 10;
let y = 20.5;
x = x + 1;
```

### Arithmetic Operations

```
result = (a + b) * c - d / e;
```

### Control Flow

**If Statements:**
```
if (x > 10) {
    print(x);
}
```

**While Loops:**
```
while (x < 100) {
    x = x + 1;
}
```

**For Loops:**
```
for (i = 0; i < 10; i = i + 1) {
    print(i);
}
```

### Comparison and Logical Operators

- Comparison: `<`, `>`, `<=`, `>=`, `==`, `!=`
- Logical: `&&`, `||`, `!`

### Output

```
print(expression);
```

## Project Structure

```
taco/
├── include/          # Header files
│   ├── codegen.h     # Code generation declarations
│   ├── lexer.h       # Lexical analyzer declarations
│   ├── logger.h      # Logging system declarations
│   ├── parser.h      # Parser and AST declarations
│   └── tac.h         # Three-address code declarations
├── src/              # Source files
│   ├── codegen.cpp   # Code generation implementation
│   ├── lexer.cpp     # Lexical analyzer implementation
│   ├── logger.cpp    # Logging system implementation
│   ├── main.cpp      # Compiler driver
│   ├── parser.cpp    # Parser implementation
│   └── tac_gen.cpp   # TAC generation implementation
├── Makefile          # Build configuration
├── LICENSE           # License file
└── README.md         # This file
```

## Compilation Pipeline

1. **Source Code** → Lexer → **Tokens**
2. **Tokens** → Parser → **Abstract Syntax Tree (AST)**
3. **AST** → TAC Generator → **Three-Address Code**
4. **TAC** → Code Generator → **C Source Code**
5. **C Code** → GCC → **Executable Binary**

## Logging and Debugging

The compiler includes a comprehensive logging system that can track:

- **Tokens**: All tokens generated during lexical analysis
- **AST**: Abstract syntax tree structure
- **TAC**: Three-address code instructions
- **Timing**: Compilation phase timing information
- **Profile**: Detailed performance profiling

Logs are written to timestamped files in the format: `compilation_DDMMYYYY_HHMMSS.log`

## License

See the `LICENSE` file for details.

## Contributing

Contributions are welcome. Please ensure:

- Code follows the existing style and conventions
- New features include appropriate test cases
- Documentation is updated accordingly

