#ifndef TAC_H
#define TAC_H

#include <string>
#include <vector>
#include <memory>
#include "parser.h"

// Three-Address Code (TAC) Instruction Types
enum class TACOpcode
{
    // Arithmetic
    ADD,        // t = a + b
    SUB,        // t = a - b
    MUL,        // t = a * b
    DIV,        // t = a / b
    
    // Assignment
    ASSIGN,     // t = a
    
    // Comparison
    LT,         // t = a < b
    GT,         // t = a > b
    LE,         // t = a <= b
    GE,         // t = a >= b
    EQ,         // t = a == b
    NE,         // t = a != b
    
    // Logical
    AND,        // t = a && b
    OR,         // t = a || b
    NOT,        // t = !a
    
    // Control flow
    LABEL,      // label:
    GOTO,       // goto label
    IF_FALSE,   // if !a goto label
    
    // I/O
    PRINT       // print a
};

// TAC Instruction
struct TACInstruction
{
    TACOpcode opcode;
    std::string result;     // Result (destination)
    std::string arg1;       // First argument
    std::string arg2;       // Second argument (optional)
    
    TACInstruction(TACOpcode op, const std::string& res = "", 
                   const std::string& a1 = "", const std::string& a2 = "")
        : opcode(op), result(res), arg1(a1), arg2(a2) {}
    
    std::string toString() const;
};

// TAC Generator
class TACGenerator
{
private:
    std::vector<TACInstruction> instructions;
    int tempCounter;
    int labelCounter;
    
    std::string newTemp();
    std::string newLabel();
    
    std::string generateExpression(const ASTNode* node);
    void generateStatement(const ASTNode* node);
    
public:
    TACGenerator();
    
    std::vector<TACInstruction> generate(const std::vector<std::unique_ptr<ASTNode>>& ast);
    void printTAC(const std::vector<TACInstruction>& tac, bool toConsole = false);
    
    const std::vector<TACInstruction>& getInstructions() const { return instructions; }
};

#endif // TAC_H
