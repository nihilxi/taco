# TACO - Three-Address Code Compiler

A lightweight compiler for arithmetic expressions with energy consumption analysis and C code generation.

## Features

- **Simple syntax** - Clean arithmetic expression language
- **TAC intermediate representation** - Three-Address Code generation
- **Energy analysis** - Instruction-level power consumption modeling
- **C code generation** - Compiles to optimized C code
- **Print statements** - Output support for computed results

## Quick Start

```bash
# Build
make

# Compile a TACO program
./taco examples/simple.taco -o output.c

# Compile and run the generated C code
gcc output.c -o output && ./output
```

## Language Syntax

```taco
// Variable assignments with arithmetic
a = 10
b = 20
sum = a + b

// Parentheses for grouping
result = (a + b) * (sum / 2)

// Print output
print(result)
```

### Supported Features

**Operators:**
- Arithmetic: `+`, `-`, `*`, `/`
- Assignment: `=`
- Grouping: `(`, `)`

**Data Types:**
- Integers and floating-point numbers
- Implicit double-precision arithmetic

**Statements:**
- Variable assignments: `variable = expression`
- Print statements: `print(expression)`

## Usage

### Command-Line Options

```bash
./taco [options] <input.taco>

Options:
  --tokens          Show lexical token stream
  --ast             Show abstract syntax tree
  --tac             Show three-address code
  --energy          Show energy consumption report
  --energy-table    Show energy cost table
  -o <file>         Output C file (default: output.c)
  --help            Show help message
```

### Examples

```bash
# View compilation stages
./taco --tokens --ast --tac examples/simple.taco

# Analyze energy consumption
./taco --energy examples/dct.taco

# Specify output file
./taco examples/fft.taco -o fft.c
```

## Project Structure

```
taco/
├── src/              # Source code
│   ├── lexer.cpp     # Tokenization
│   ├── parser.cpp    # Syntax analysis and AST
│   ├── tac_gen.cpp   # TAC generation
│   ├── energy.cpp    # Energy modeling
│   ├── codegen.cpp   # C code generation
│   └── main.cpp      # Compiler driver
├── include/          # Headers
├── examples/         # Sample programs
│   ├── simple.taco
│   ├── fft.taco      # Fast Fourier Transform
│   ├── dct.taco      # Discrete Cosine Transform
│   └── rectangle.taco
└── Makefile
```

## Compilation Pipeline

```
TACO source → Lexer → Parser → AST → TAC Generator → Energy Analyzer
                                                            ↓
                                                     Energy Report
                                                            ↓
                                                    C Code Generator
                                                            ↓
                                                         C code
```

## Energy Model

Energy costs are based on typical CPU instruction latencies:

| Operation | Energy Cost | Description |
|-----------|-------------|-------------|
| ADD, SUB  | 1.5 units   | Basic arithmetic |
| MUL       | 3.5 units   | Multiplication |
| DIV       | 20.5 units  | Division (expensive) |
| ASSIGN    | 1.0 unit    | Register move |
| PRINT     | 12.0 units  | I/O operation |

View full energy table:
```bash
./taco --energy-table
```

## Example: Rectangle Calculations

**Input** (`rectangle.taco`):
```taco
width = 10.5
height = 7.3

area = width * height
print(area)

perimeter = 2 * (width + height)
print(perimeter)
```

**Output**:
```
76.65
35.6
```

**Energy Analysis**:
```bash
./taco --energy rectangle.taco
# Total energy: 60 units
# - Multiplication: 29.17%
# - Addition: 12.50%
# - I/O: 40.00%
```

## Build System

```bash
make           # Build compiler
make clean     # Remove build artifacts
make test      # Run basic tests
make benchmark # Run performance benchmarks
```

## Benchmarking

The TACO compiler includes a comprehensive benchmark suite for performance analysis:

```bash
# Run standard benchmark (5 iterations)
make benchmark

# Quick benchmark (1 iteration)
make benchmark-quick

# Detailed benchmark (10 iterations)
make benchmark-detailed

# View results
cat benchmarks/results/latest.txt
```

The benchmark suite measures:
- **Compilation time** - How fast the compiler processes code
- **TAC instruction count** - Efficiency of intermediate representation
- **Energy consumption** - Estimated power usage
- **Execution time** - Performance of generated code
- **Code quality** - Size and structure of generated C code

See [`benchmarks/README.md`](benchmarks/README.md) for detailed documentation.

## Requirements

- **g++** with C++14 support
- **make**
- **gcc** (for compiling generated C code)

## Implementation Details

### Three-Address Code

TACO generates TAC where each instruction performs at most one operation:

```taco
result = (a + b) * (c - d)
```

Becomes:
```
t0 = a + b
t1 = c - d
t2 = t0 * t1
result = t2
```

### Generated C Code

All variables are declared as `double` with automatic temporary variable management:

```c
int main() {
    double a = 0.0, b = 0.0, result = 0.0;
    double t0 = 0.0, t1 = 0.0;
    
    a = 10;
    b = 20;
    t0 = a + b;
    result = t0;
    printf("%g\n", result);
    
    return 0;
}
```

## Future Enhancements

- Control flow statements (if/while/for)
- Functions and scope
- Type system
- Arrays and data structures
- Advanced optimizations

## License

MIT License

