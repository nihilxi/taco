#include <iostream>
#include <fstream>
#include "../include/parser.h"
#include "../include/lexer.h"
#include "../include/logger.h"

// Constructor
Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens), current(0) {}

// Peek at current token without consuming it
Token Parser::peek()
{
    if (current < tokens.size())
        return tokens[current];
    return tokens.back(); // Return EOF token
}

// Consume and return current token
Token Parser::advance()
{
    if (current < tokens.size())
        return tokens[current++];
    return tokens.back();
}

// Check if current token matches type without consuming
bool Parser::check(TokenType type)
{
    if (current >= tokens.size())
        return false;
    return peek().type == type;
}

// Check and consume if matches
bool Parser::match(TokenType type)
{
    if (check(type))
    {
        advance();
        return true;
    }
    return false;
}

// Parse primary expressions (numbers, identifiers, and parenthesized expressions)
std::unique_ptr<ASTNode> Parser::parsePrimary()
{
    Token token = peek();
    
    // Handle parenthesized expressions
    if (token.type == TokenType::LPAREN)
    {
        advance(); // consume '('
        auto expr = parseExpression();
        
        if (!match(TokenType::RPAREN))
        {
            logger << "Error: Expected ')' at line " << peek().line << std::endl;
        }
        
        return expr;
    }
    
    if (token.type == TokenType::NUMBER)
    {
        advance();
        return std::make_unique<NumberNode>(token.value);
    }
    else if (token.type == TokenType::IDENTIFIER)
    {
        advance();
        return std::make_unique<IdentifierNode>(token.value);
    }
    
    logger << "Error: Expected number or identifier at line " << token.line << std::endl;
    return nullptr;
}

// Parse term (multiplication and division)
std::unique_ptr<ASTNode> Parser::parseTerm()
{
    auto left = parsePrimary();
    
    while (check(TokenType::MUL) || check(TokenType::DIV))
    {
        Token op = advance();
        auto right = parsePrimary();
        left = std::make_unique<BinaryOpNode>(op.type, std::move(left), std::move(right));
    }
    
    return left;
}

// Parse expression (addition and subtraction)
std::unique_ptr<ASTNode> Parser::parseExpression()
{
    auto left = parseTerm();
    
    while (check(TokenType::ADD) || check(TokenType::SUB))
    {
        Token op = advance();
        auto right = parseTerm();
        left = std::make_unique<BinaryOpNode>(op.type, std::move(left), std::move(right));
    }
    
    return left;
}

// Parse assignment statement
std::unique_ptr<ASTNode> Parser::parseAssignment()
{
    Token token = peek();
    
    if (token.type == TokenType::IDENTIFIER)
    {
        std::string identifier = token.value;
        advance();
        
        if (match(TokenType::ASSIGN))
        {
            auto expression = parseExpression();
            return std::make_unique<AssignmentNode>(identifier, std::move(expression));
        }
        else
        {
            logger << "Error: Expected '=' after identifier at line " << token.line << std::endl;
            return nullptr;
        }
    }
    
    logger << "Error: Expected identifier at line " << token.line << std::endl;
    return nullptr;
}

// Parse print statement
std::unique_ptr<ASTNode> Parser::parsePrint()
{
    advance(); // consume 'print'
    
    if (!match(TokenType::LPAREN))
    {
        logger << "Error: Expected '(' after 'print' at line " << peek().line << std::endl;
        return nullptr;
    }
    
    auto expression = parseExpression();
    
    if (!match(TokenType::RPAREN))
    {
        logger << "Error: Expected ')' after expression at line " << peek().line << std::endl;
        return nullptr;
    }
    
    return std::make_unique<PrintNode>(std::move(expression));
}

// Parse a statement (assignment or print)
std::unique_ptr<ASTNode> Parser::parseStatement()
{
    Token token = peek();
    
    if (token.type == TokenType::PRINT)
    {
        return parsePrint();
    }
    else if (token.type == TokenType::IDENTIFIER)
    {
        return parseAssignment();
    }
    
    logger << "Error: Expected statement at line " << token.line << std::endl;
    return nullptr;
}

// Parse the entire program
std::vector<std::unique_ptr<ASTNode>> Parser::parse()
{
    std::vector<std::unique_ptr<ASTNode>> statements;
    
    while (!check(TokenType::END_OF_FILE))
    {
        auto statement = parseStatement();
        if (statement)
        {
            statements.push_back(std::move(statement));
        }
        else
        {
            // Skip to next statement on error
            advance();
        }
    }
    
    return statements;
}

// Print AST for debugging
void Parser::printAST(const ASTNode* node, int indent, bool toConsole)
{
    if (!node)
        return;
        
    std::string indentation(indent * 2, ' ');
    
    switch (node->type)
    {
        case ASTNodeType::NUMBER:
        {
            const NumberNode* num = static_cast<const NumberNode*>(node);
            if (toConsole)
                std::cout << indentation << "Number: " << num->value << std::endl;
            else
                logger << indentation << "Number: " << num->value << std::endl;
            break;
        }
        case ASTNodeType::IDENTIFIER:
        {
            const IdentifierNode* id = static_cast<const IdentifierNode*>(node);
            if (toConsole)
                std::cout << indentation << "Identifier: " << id->name << std::endl;
            else
                logger << indentation << "Identifier: " << id->name << std::endl;
            break;
        }
        case ASTNodeType::BINARY_OP:
        {
            const BinaryOpNode* binop = static_cast<const BinaryOpNode*>(node);
            if (toConsole)
            {
                std::cout << indentation << "BinaryOp: ";
                switch (binop->op)
                {
                    case TokenType::ADD: std::cout << "+" << std::endl; break;
                    case TokenType::SUB: std::cout << "-" << std::endl; break;
                    case TokenType::MUL: std::cout << "*" << std::endl; break;
                    case TokenType::DIV: std::cout << "/" << std::endl; break;
                    default: std::cout << "?" << std::endl; break;
                }
            }
            else
            {
                logger << indentation << "BinaryOp: ";
                switch (binop->op)
                {
                    case TokenType::ADD: logger << "+" << std::endl; break;
                    case TokenType::SUB: logger << "-" << std::endl; break;
                    case TokenType::MUL: logger << "*" << std::endl; break;
                    case TokenType::DIV: logger << "/" << std::endl; break;
                    default: logger << "?" << std::endl; break;
                }
            }
            printAST(binop->left.get(), indent + 1, toConsole);
            printAST(binop->right.get(), indent + 1, toConsole);
            break;
        }
        case ASTNodeType::ASSIGNMENT:
        {
            const AssignmentNode* assign = static_cast<const AssignmentNode*>(node);
            if (toConsole)
                std::cout << indentation << "Assignment: " << assign->identifier << std::endl;
            else
                logger << indentation << "Assignment: " << assign->identifier << std::endl;
            printAST(assign->expression.get(), indent + 1, toConsole);
            break;
        }
        case ASTNodeType::PRINT:
        {
            const PrintNode* print = static_cast<const PrintNode*>(node);
            if (toConsole)
                std::cout << indentation << "Print:" << std::endl;
            else
                logger << indentation << "Print:" << std::endl;
            printAST(print->expression.get(), indent + 1, toConsole);
            break;
        }
    }
}
