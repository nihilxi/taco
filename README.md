# TACO - Three-Address Code Compiler

A TACO language compiler with energy analysis and C code generation.

## Overview

TACO is an experimental compiler that:
1. Parses a simple imperative programming language
2. Generates Three-Address Code (TAC) intermediate representation
3. Performs instruction-level energy consumption analysis
4. Generates compilation-ready C code

## Project Structure

```
taco/
├── include/          # Header files
│   ├── lexer.h       # Lexical analyzer
│   ├── parser.h      # Parser and AST definitions
│   ├── tac.h         # TAC generator
│   ├── energy.h      # Energy model
│   └── codegen.h     # C code generator
├── src/              # Implementation files
│   ├── lexer.cpp
│   ├── parser.cpp
│   ├── tac_gen.cpp
│   ├── energy.cpp
│   ├── codegen.cpp
│   └── main.cpp      # Main compiler driver
├── grammar/          # Language specification
│   ├── grammar.ebnf  # EBNF grammar
│   └── language_spec.md
├── examples/         # Example programs
│   ├── simple.taco   # Basic example
│   ├── fft.taco      # Fast Fourier Transform
│   └── dct.taco      # Discrete Cosine Transform
├── tests/            # Test suite
├── benchmarks/       # Performance benchmarks
└── Makefile
```

## Build Instructions

### Requirements
- g++ with C++14 support
- make
- gcc (for compiling generated C code)

### Building the Compiler

```bash
make
```

This produces the `taco` compiler binary in the project root.

### Running Tests

```bash
# Basic test
make test

# FFT test
make test-fft

# DCT test
make test-dct

# Full test suite
make test-all
```

## Usage

### Basic Compilation

```bash
./taco program.taco
```

Compiles `program.taco` and generates `output.c`.

### Command-Line Options

```bash
./taco [options] <input.taco>

Options:
  --tokens          Display token stream
  --ast             Display abstract syntax tree
  --tac             Display three-address code
  --energy          Display energy consumption report
  --energy-table    Display energy cost table
  -o <file>         Specify output C file (default: output.c)
  --help            Display help message
```

### Example Workflow

```bash
# Compile with full diagnostics
./taco --tokens --ast --tac --energy examples/simple.taco -o simple.c

# Compile generated C code
gcc simple.c -o simple -lm

# Execute
./simple
```

## TACO Language Reference

### Syntax Example

```taco
// Simple arithmetic
a = 5
b = 3
c = a + b
d = a * b
result = c + d
```

### Supported Operators

**Arithmetic:**
- `+` - addition
- `-` - subtraction
- `*` - multiplication
- `/` - division
- `%` - modulo

**Comparison** (planned):
- `==`, `!=`, `<`, `<=`, `>`, `>=`

**Logical** (planned):
- `&&`, `||`, `!`

### Current Limitations

The current parser implementation supports:
- Variable assignments
- Arithmetic expressions
- Variables and numeric literals

Planned features:
- Conditional statements (if/else)
- Loop constructs (while, for)
- Functions
- Arrays
- Type system

## Energy Model

The compiler estimates energy consumption based on TAC instruction costs.

### Instruction Costs (energy units)

| Operation | Base Cost | Memory Cost | Total |
|-----------|-----------|-------------|-------|
| ADD/SUB   | 1.0       | 0.5         | 1.5   |
| MUL       | 3.0       | 0.5         | 3.5   |
| DIV/MOD   | 20.0      | 0.5         | 20.5  |
| ASSIGN    | 0.5       | 0.5         | 1.0   |
| JUMP      | 5.0       | 0.0         | 5.0   |
| CALL      | 10.0      | 2.0         | 12.0  |

Full cost table: `./taco --energy-table`

### Energy Analysis Report

```bash
./taco --energy examples/dct.taco
```

Generates a detailed report including:
- Execution count per instruction
- Energy cost per operation type
- Total program energy consumption
- Percentage breakdown by instruction type

## Compilation Pipeline

```
Source code (.taco)
        ↓
    [Lexer]
        ↓
    Token stream
        ↓
    [Parser]
        ↓
    AST (Abstract Syntax Tree)
        ↓
    [TAC Generator]
        ↓
    TAC (Three-Address Code)
        ↓
    [Energy Analyzer] ← Energy model
        ↓
    Energy report
        ↓
    [C Code Generator]
        ↓
    C code (.c)
        ↓
    [gcc]
        ↓
    Executable binary
```

## Code Transformation Examples

### TACO Source
```taco
a = 5
b = 3
c = a + b
```

### TAC Intermediate Representation
```
0: a = 5
1: b = 3
2: t0 = a + b
3: c = t0
```

### Generated C Code
```c
int main() {
    // User variables
    double a = 0.0;
    double b = 0.0;
    double c = 0.0;

    // Temporary variables
    double t0 = 0.0;

    // Program code
    a = 5;
    b = 3;
    t0 = a + b;
    c = t0;

    return 0;
}
```

## Transform Implementations

### FFT (Fast Fourier Transform)
```bash
./taco --tac --energy examples/fft.taco -o fft.c
gcc fft.c -o fft -lm
./fft
```

Implements a simplified 8-point Cooley-Tukey FFT algorithm.

### DCT (Discrete Cosine Transform)
```bash
./taco --tac --energy examples/dct.taco -o dct.c
gcc dct.c -o dct -lm
./dct
```

Implements 8-point DCT Type-II (used in JPEG compression).

## Energy Performance Analysis

Compare energy consumption between FFT and DCT:

```bash
# FFT analysis
./taco --energy examples/fft.taco

# DCT analysis
./taco --energy examples/dct.taco
```

Analysis reveals:
- Comparative energy efficiency between transforms
- Dominant operations in energy consumption
- Potential optimization opportunities

## License

MIT License. Developed as part of an engineering thesis.

