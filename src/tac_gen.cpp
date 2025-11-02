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
        case TACOpcode::MOD:
            oss << result << " = " << arg1 << " % " << arg2;
            break;
        case TACOpcode::NEG:
            oss << result << " = -" << arg1;
            break;
        case TACOpcode::AND:
            oss << result << " = " << arg1 << " && " << arg2;
            break;
        case TACOpcode::OR:
            oss << result << " = " << arg1 << " || " << arg2;
            break;
        case TACOpcode::NOT:
            oss << result << " = !" << arg1;
            break;
        case TACOpcode::EQ:
            oss << result << " = " << arg1 << " == " << arg2;
            break;
        case TACOpcode::NEQ:
            oss << result << " = " << arg1 << " != " << arg2;
            break;
        case TACOpcode::LT:
            oss << result << " = " << arg1 << " < " << arg2;
            break;
        case TACOpcode::LTE:
            oss << result << " = " << arg1 << " <= " << arg2;
            break;
        case TACOpcode::GT:
            oss << result << " = " << arg1 << " > " << arg2;
            break;
        case TACOpcode::GTE:
            oss << result << " = " << arg1 << " >= " << arg2;
            break;
        case TACOpcode::ASSIGN:
            oss << result << " = " << arg1;
            break;
        case TACOpcode::LABEL:
            oss << result << ":";
            break;
        case TACOpcode::GOTO:
            oss << "goto " << result;
            break;
        case TACOpcode::IF_FALSE:
            oss << "if_false " << arg1 << " goto " << result;
            break;
        case TACOpcode::IF_TRUE:
            oss << "if_true " << arg1 << " goto " << result;
            break;
        case TACOpcode::PARAM:
            oss << "param " << arg1;
            break;
        case TACOpcode::CALL:
            oss << result << " = call " << arg1 << ", " << arg2;
            break;
        case TACOpcode::RETURN:
            if (!arg1.empty())
                oss << "return " << arg1;
            else
                oss << "return";
            break;
        case TACOpcode::INDEX:
            oss << result << " = " << arg1 << "[" << arg2 << "]";
            break;
        case TACOpcode::STORE:
            oss << result << "[" << arg1 << "] = " << arg2;
            break;
        case TACOpcode::NOP:
            oss << "nop";
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
                case TokenType::MOD: opcode = TACOpcode::MOD; break;
                case TokenType::EQ:  opcode = TACOpcode::EQ;  break;
                case TokenType::NEQ: opcode = TACOpcode::NEQ; break;
                case TokenType::LT:  opcode = TACOpcode::LT;  break;
                case TokenType::LTE: opcode = TACOpcode::LTE; break;
                case TokenType::GT:  opcode = TACOpcode::GT;  break;
                case TokenType::GTE: opcode = TACOpcode::GTE; break;
                case TokenType::AND: opcode = TACOpcode::AND; break;
                case TokenType::OR:  opcode = TACOpcode::OR;  break;
                default: opcode = TACOpcode::NOP; break;
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
