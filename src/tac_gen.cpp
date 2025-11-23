#include <iostream>
#include <sstream>
#include "../include/tac.h"
#include "../include/parser.h"
#include "../include/logger.h"

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
        case TACOpcode::LT:
            oss << result << " = " << arg1 << " < " << arg2;
            break;
        case TACOpcode::GT:
            oss << result << " = " << arg1 << " > " << arg2;
            break;
        case TACOpcode::LE:
            oss << result << " = " << arg1 << " <= " << arg2;
            break;
        case TACOpcode::GE:
            oss << result << " = " << arg1 << " >= " << arg2;
            break;
        case TACOpcode::EQ:
            oss << result << " = " << arg1 << " == " << arg2;
            break;
        case TACOpcode::NE:
            oss << result << " = " << arg1 << " != " << arg2;
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
        case TACOpcode::LABEL:
            oss << result << ":";
            break;
        case TACOpcode::GOTO:
            oss << "goto " << result;
            break;
        case TACOpcode::IF_FALSE:
            oss << "if !" << arg1 << " goto " << result;
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
        
        case ASTNodeType::COMPARISON:
        {
            const ComparisonNode* comp = static_cast<const ComparisonNode*>(node);
            
            std::string left = generateExpression(comp->left.get());
            std::string right = generateExpression(comp->right.get());
            std::string temp = newTemp();
            
            TACOpcode opcode;
            switch (comp->op)
            {
                case TokenType::LT: opcode = TACOpcode::LT; break;
                case TokenType::GT: opcode = TACOpcode::GT; break;
                case TokenType::LE: opcode = TACOpcode::LE; break;
                case TokenType::GE: opcode = TACOpcode::GE; break;
                case TokenType::EQ: opcode = TACOpcode::EQ; break;
                case TokenType::NE: opcode = TACOpcode::NE; break;
                default: return "";
            }
            
            instructions.push_back(TACInstruction(opcode, temp, left, right));
            return temp;
        }
        
        case ASTNodeType::LOGICAL_OP:
        {
            const LogicalOpNode* logic = static_cast<const LogicalOpNode*>(node);
            
            std::string left = generateExpression(logic->left.get());
            std::string right = generateExpression(logic->right.get());
            std::string temp = newTemp();
            
            TACOpcode opcode;
            switch (logic->op)
            {
                case TokenType::AND: opcode = TACOpcode::AND; break;
                case TokenType::OR: opcode = TACOpcode::OR; break;
                default: return "";
            }
            
            instructions.push_back(TACInstruction(opcode, temp, left, right));
            return temp;
        }
        
        case ASTNodeType::UNARY_OP:
        {
            const UnaryOpNode* unary = static_cast<const UnaryOpNode*>(node);
            
            std::string operand = generateExpression(unary->operand.get());
            std::string temp = newTemp();
            
            if (unary->op == TokenType::NOT)
            {
                instructions.push_back(TACInstruction(TACOpcode::NOT, temp, operand));
            }
            
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
        
        case ASTNodeType::IF_STATEMENT:
        {
            const IfNode* ifNode = static_cast<const IfNode*>(node);
            
            std::string condition = generateExpression(ifNode->condition.get());
            std::string elseLabel = newLabel();
            std::string endLabel = newLabel();
            
            // if !condition goto elseLabel
            instructions.push_back(TACInstruction(TACOpcode::IF_FALSE, elseLabel, condition));
            
            // Then branch
            generateStatement(ifNode->thenBranch.get());
            
            if (ifNode->elseBranch)
            {
                // goto endLabel
                instructions.push_back(TACInstruction(TACOpcode::GOTO, endLabel));
                
                // elseLabel:
                instructions.push_back(TACInstruction(TACOpcode::LABEL, elseLabel));
                
                // Else branch
                generateStatement(ifNode->elseBranch.get());
                
                // endLabel:
                instructions.push_back(TACInstruction(TACOpcode::LABEL, endLabel));
            }
            else
            {
                // elseLabel:
                instructions.push_back(TACInstruction(TACOpcode::LABEL, elseLabel));
            }
            break;
        }
        
        case ASTNodeType::WHILE_LOOP:
        {
            const WhileNode* whileNode = static_cast<const WhileNode*>(node);
            
            std::string startLabel = newLabel();
            std::string endLabel = newLabel();
            
            // startLabel:
            instructions.push_back(TACInstruction(TACOpcode::LABEL, startLabel));
            
            std::string condition = generateExpression(whileNode->condition.get());
            
            // if !condition goto endLabel
            instructions.push_back(TACInstruction(TACOpcode::IF_FALSE, endLabel, condition));
            
            // Body
            generateStatement(whileNode->body.get());
            
            // goto startLabel
            instructions.push_back(TACInstruction(TACOpcode::GOTO, startLabel));
            
            // endLabel:
            instructions.push_back(TACInstruction(TACOpcode::LABEL, endLabel));
            break;
        }
        
        case ASTNodeType::FOR_LOOP:
        {
            const ForNode* forNode = static_cast<const ForNode*>(node);
            
            // Initialization
            if (forNode->initialization)
            {
                generateStatement(forNode->initialization.get());
            }
            
            std::string startLabel = newLabel();
            std::string endLabel = newLabel();
            
            // startLabel:
            instructions.push_back(TACInstruction(TACOpcode::LABEL, startLabel));
            
            // Condition check
            if (forNode->condition)
            {
                std::string condition = generateExpression(forNode->condition.get());
                // if !condition goto endLabel
                instructions.push_back(TACInstruction(TACOpcode::IF_FALSE, endLabel, condition));
            }
            
            // Body
            generateStatement(forNode->body.get());
            
            // Increment
            if (forNode->increment)
            {
                generateStatement(forNode->increment.get());
            }
            
            // goto startLabel
            instructions.push_back(TACInstruction(TACOpcode::GOTO, startLabel));
            
            // endLabel:
            instructions.push_back(TACInstruction(TACOpcode::LABEL, endLabel));
            break;
        }
        
        case ASTNodeType::BLOCK:
        {
            const BlockNode* block = static_cast<const BlockNode*>(node);
            for (const auto& stmt : block->statements)
            {
                generateStatement(stmt.get());
            }
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
void TACGenerator::printTAC(const std::vector<TACInstruction>& tac, bool toConsole)
{
    if (toConsole)
    {
        std::cout << "=== THREE-ADDRESS CODE ===" << std::endl;
        int line = 1;
        for (const auto& instr : tac)
        {
            std::cout << line++ << ": " << instr.toString() << std::endl;
        }
    }
    else
    {
        logger << "=== THREE-ADDRESS CODE ===" << std::endl;
        int line = 1;
        for (const auto& instr : tac)
        {
            logger << line++ << ": " << instr.toString() << std::endl;
        }
    }
}
