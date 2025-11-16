#include <iostream>
#include <sstream>
#include "../include/tac.h"
#include "../include/parser.h"

// Convert TAC instruction to string
std::string TACInstruction::toString() const
{
    std::ostringstream oss;
    
    switch (opcode)
    {
        case TACOpcode::ADD:
            oss << result << " = " << arg1 << " + " << arg2;
            break;
        case TACOpcode::SUB:
            oss << result << " = " << arg1 << " - " << arg2;
            break;
        case TACOpcode::MUL:
            oss << result << " = " << arg1 << " * " << arg2;
            break;
        case TACOpcode::DIV:
            oss << result << " = " << arg1 << " / " << arg2;
            break;
        case TACOpcode::ASSIGN:
            oss << result << " = " << arg1;
            break;
        case TACOpcode::PRINT:
            oss << "print " << arg1;
            break;
        default:
            break;
    }
    
    return oss.str();
}

// Constructor
TACGenerator::TACGenerator() : tempCounter(0), labelCounter(0) {}

// Generate new temporary variable
std::string TACGenerator::newTemp()
{
    return "t" + std::to_string(tempCounter++);
}

// Generate new label
std::string TACGenerator::newLabel()
{
    return "L" + std::to_string(labelCounter++);
}

// Generate TAC for expressions
std::string TACGenerator::generateExpression(const ASTNode* node)
{
    if (!node)
        return "";
    
    switch (node->type)
    {
        case ASTNodeType::NUMBER:
        {
            const NumberNode* num = static_cast<const NumberNode*>(node);
            return num->value;
        }
        
        case ASTNodeType::IDENTIFIER:
        {
            const IdentifierNode* id = static_cast<const IdentifierNode*>(node);
            return id->name;
        }
        
        case ASTNodeType::BINARY_OP:
        {
            const BinaryOpNode* binop = static_cast<const BinaryOpNode*>(node);
            
            std::string left = generateExpression(binop->left.get());
            std::string right = generateExpression(binop->right.get());
            std::string temp = newTemp();
            
            TACOpcode opcode;
            switch (binop->op)
            {
                case TokenType::ADD: opcode = TACOpcode::ADD; break;
                case TokenType::SUB: opcode = TACOpcode::SUB; break;
                case TokenType::MUL: opcode = TACOpcode::MUL; break;
                case TokenType::DIV: opcode = TACOpcode::DIV; break;
                default: return ""; // Unsupported operation
            }
            
            instructions.push_back(TACInstruction(opcode, temp, left, right));
            return temp;
        }
        
        default:
            return "";
    }
}

// Generate TAC for statements
void TACGenerator::generateStatement(const ASTNode* node)
{
    if (!node)
        return;
    
    switch (node->type)
    {
        case ASTNodeType::ASSIGNMENT:
        {
            const AssignmentNode* assign = static_cast<const AssignmentNode*>(node);
            std::string expr = generateExpression(assign->expression.get());
            instructions.push_back(TACInstruction(TACOpcode::ASSIGN, assign->identifier, expr));
            break;
        }
        
        case ASTNodeType::PRINT:
        {
            const PrintNode* print = static_cast<const PrintNode*>(node);
            std::string expr = generateExpression(print->expression.get());
            instructions.push_back(TACInstruction(TACOpcode::PRINT, "", expr));
            break;
        }
        
        default:
            break;
    }
}

// Generate TAC from AST
std::vector<TACInstruction> TACGenerator::generate(const std::vector<std::unique_ptr<ASTNode>>& ast)
{
    instructions.clear();
    tempCounter = 0;
    labelCounter = 0;
    
    for (const auto& statement : ast)
    {
        generateStatement(statement.get());
    }
    
    return instructions;
}

// Print TAC
void TACGenerator::printTAC(const std::vector<TACInstruction>& tac)
{
    std::cout << "=== THREE-ADDRESS CODE ===" << std::endl;
    int line = 0;
    for (const auto& instr : tac)
    {
        std::cout << line++ << ": " << instr.toString() << std::endl;
    }
}
