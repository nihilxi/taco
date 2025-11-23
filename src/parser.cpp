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

// Parse comparison expressions
std::unique_ptr<ASTNode> Parser::parseComparison()
{
    auto left = parseExpression();
    
    while (check(TokenType::LT) || check(TokenType::GT) || 
           check(TokenType::LE) || check(TokenType::GE) ||
           check(TokenType::EQ) || check(TokenType::NE))
    {
        Token op = advance();
        auto right = parseExpression();
        left = std::make_unique<ComparisonNode>(op.type, std::move(left), std::move(right));
    }
    
    return left;
}

// Parse unary expressions (NOT)
std::unique_ptr<ASTNode> Parser::parseUnary()
{
    if (check(TokenType::NOT))
    {
        Token op = advance();
        auto operand = parseUnary();
        return std::make_unique<UnaryOpNode>(op.type, std::move(operand));
    }
    
    return parseComparison();
}

// Parse logical AND
std::unique_ptr<ASTNode> Parser::parseLogicalAnd()
{
    auto left = parseUnary();
    
    while (check(TokenType::AND))
    {
        Token op = advance();
        auto right = parseUnary();
        left = std::make_unique<LogicalOpNode>(op.type, std::move(left), std::move(right));
    }
    
    return left;
}

// Parse logical OR
std::unique_ptr<ASTNode> Parser::parseLogicalOr()
{
    auto left = parseLogicalAnd();
    
    while (check(TokenType::OR))
    {
        Token op = advance();
        auto right = parseLogicalAnd();
        left = std::make_unique<LogicalOpNode>(op.type, std::move(left), std::move(right));
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
            auto expression = parseLogicalOr();
            
            // Optional semicolon
            match(TokenType::SEMICOLON);
            
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
    
    auto expression = parseLogicalOr();
    
    if (!match(TokenType::RPAREN))
    {
        logger << "Error: Expected ')' after expression at line " << peek().line << std::endl;
        return nullptr;
    }
    
    // Optional semicolon
    match(TokenType::SEMICOLON);
    
    return std::make_unique<PrintNode>(std::move(expression));
}

// Parse block { statements }
std::unique_ptr<ASTNode> Parser::parseBlock()
{
    if (!match(TokenType::LBRACE))
    {
        logger << "Error: Expected '{' at line " << peek().line << std::endl;
        return nullptr;
    }
    
    auto block = std::make_unique<BlockNode>();
    
    while (!check(TokenType::RBRACE) && !check(TokenType::END_OF_FILE))
    {
        auto statement = parseStatement();
        if (statement)
        {
            block->statements.push_back(std::move(statement));
        }
        else
        {
            // Skip to next statement on error
            advance();
        }
    }
    
    if (!match(TokenType::RBRACE))
    {
        logger << "Error: Expected '}' at line " << peek().line << std::endl;
        return nullptr;
    }
    
    return block;
}

// Parse if statement
std::unique_ptr<ASTNode> Parser::parseIf()
{
    advance(); // consume 'if'
    
    if (!match(TokenType::LPAREN))
    {
        logger << "Error: Expected '(' after 'if' at line " << peek().line << std::endl;
        return nullptr;
    }
    
    auto condition = parseLogicalOr();
    
    if (!match(TokenType::RPAREN))
    {
        logger << "Error: Expected ')' after condition at line " << peek().line << std::endl;
        return nullptr;
    }
    
    std::unique_ptr<ASTNode> thenBranch;
    if (check(TokenType::LBRACE))
    {
        thenBranch = parseBlock();
    }
    else
    {
        thenBranch = parseStatement();
    }
    
    std::unique_ptr<ASTNode> elseBranch = nullptr;
    if (match(TokenType::ELSE))
    {
        if (check(TokenType::LBRACE))
        {
            elseBranch = parseBlock();
        }
        else
        {
            elseBranch = parseStatement();
        }
    }
    
    return std::make_unique<IfNode>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

// Parse while loop
std::unique_ptr<ASTNode> Parser::parseWhile()
{
    advance(); // consume 'while'
    
    if (!match(TokenType::LPAREN))
    {
        logger << "Error: Expected '(' after 'while' at line " << peek().line << std::endl;
        return nullptr;
    }
    
    auto condition = parseLogicalOr();
    
    if (!match(TokenType::RPAREN))
    {
        logger << "Error: Expected ')' after condition at line " << peek().line << std::endl;
        return nullptr;
    }
    
    std::unique_ptr<ASTNode> body;
    if (check(TokenType::LBRACE))
    {
        body = parseBlock();
    }
    else
    {
        body = parseStatement();
    }
    
    return std::make_unique<WhileNode>(std::move(condition), std::move(body));
}

// Parse for loop
std::unique_ptr<ASTNode> Parser::parseFor()
{
    advance(); // consume 'for'
    
    if (!match(TokenType::LPAREN))
    {
        logger << "Error: Expected '(' after 'for' at line " << peek().line << std::endl;
        return nullptr;
    }
    
    // Parse initialization
    std::unique_ptr<ASTNode> init = nullptr;
    if (!check(TokenType::SEMICOLON))
    {
        init = parseAssignment();
    }
    else
    {
        match(TokenType::SEMICOLON);
    }
    
    // Parse condition
    std::unique_ptr<ASTNode> condition = nullptr;
    if (!check(TokenType::SEMICOLON))
    {
        condition = parseLogicalOr();
    }
    
    if (!match(TokenType::SEMICOLON))
    {
        logger << "Error: Expected ';' after condition at line " << peek().line << std::endl;
        return nullptr;
    }
    
    // Parse increment
    std::unique_ptr<ASTNode> increment = nullptr;
    if (!check(TokenType::RPAREN))
    {
        // Parse increment expression (assignment without semicolon)
        Token token = peek();
        if (token.type == TokenType::IDENTIFIER)
        {
            std::string identifier = token.value;
            advance();
            
            if (match(TokenType::ASSIGN))
            {
                auto expression = parseLogicalOr();
                increment = std::make_unique<AssignmentNode>(identifier, std::move(expression));
            }
        }
    }
    
    if (!match(TokenType::RPAREN))
    {
        logger << "Error: Expected ')' after for clauses at line " << peek().line << std::endl;
        return nullptr;
    }
    
    std::unique_ptr<ASTNode> body;
    if (check(TokenType::LBRACE))
    {
        body = parseBlock();
    }
    else
    {
        body = parseStatement();
    }
    
    return std::make_unique<ForNode>(std::move(init), std::move(condition), std::move(increment), std::move(body));
}

// Parse a statement (assignment or print)
std::unique_ptr<ASTNode> Parser::parseStatement()
{
    Token token = peek();
    
    if (token.type == TokenType::PRINT)
    {
        return parsePrint();
    }
    else if (token.type == TokenType::IF)
    {
        return parseIf();
    }
    else if (token.type == TokenType::WHILE)
    {
        return parseWhile();
    }
    else if (token.type == TokenType::FOR)
    {
        return parseFor();
    }
    else if (token.type == TokenType::LBRACE)
    {
        return parseBlock();
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
        case ASTNodeType::COMPARISON:
        {
            const ComparisonNode* comp = static_cast<const ComparisonNode*>(node);
            if (toConsole)
            {
                std::cout << indentation << "Comparison: ";
                switch (comp->op)
                {
                    case TokenType::LT: std::cout << "<" << std::endl; break;
                    case TokenType::GT: std::cout << ">" << std::endl; break;
                    case TokenType::LE: std::cout << "<=" << std::endl; break;
                    case TokenType::GE: std::cout << ">=" << std::endl; break;
                    case TokenType::EQ: std::cout << "==" << std::endl; break;
                    case TokenType::NE: std::cout << "!=" << std::endl; break;
                    default: std::cout << "?" << std::endl; break;
                }
            }
            else
            {
                logger << indentation << "Comparison: ";
                switch (comp->op)
                {
                    case TokenType::LT: logger << "<" << std::endl; break;
                    case TokenType::GT: logger << ">" << std::endl; break;
                    case TokenType::LE: logger << "<=" << std::endl; break;
                    case TokenType::GE: logger << ">=" << std::endl; break;
                    case TokenType::EQ: logger << "==" << std::endl; break;
                    case TokenType::NE: logger << "!=" << std::endl; break;
                    default: logger << "?" << std::endl; break;
                }
            }
            printAST(comp->left.get(), indent + 1, toConsole);
            printAST(comp->right.get(), indent + 1, toConsole);
            break;
        }
        case ASTNodeType::LOGICAL_OP:
        {
            const LogicalOpNode* logic = static_cast<const LogicalOpNode*>(node);
            if (toConsole)
            {
                std::cout << indentation << "LogicalOp: ";
                switch (logic->op)
                {
                    case TokenType::AND: std::cout << "&&" << std::endl; break;
                    case TokenType::OR: std::cout << "||" << std::endl; break;
                    default: std::cout << "?" << std::endl; break;
                }
            }
            else
            {
                logger << indentation << "LogicalOp: ";
                switch (logic->op)
                {
                    case TokenType::AND: logger << "&&" << std::endl; break;
                    case TokenType::OR: logger << "||" << std::endl; break;
                    default: logger << "?" << std::endl; break;
                }
            }
            printAST(logic->left.get(), indent + 1, toConsole);
            printAST(logic->right.get(), indent + 1, toConsole);
            break;
        }
        case ASTNodeType::UNARY_OP:
        {
            const UnaryOpNode* unary = static_cast<const UnaryOpNode*>(node);
            if (toConsole)
            {
                std::cout << indentation << "UnaryOp: ";
                switch (unary->op)
                {
                    case TokenType::NOT: std::cout << "!" << std::endl; break;
                    default: std::cout << "?" << std::endl; break;
                }
            }
            else
            {
                logger << indentation << "UnaryOp: ";
                switch (unary->op)
                {
                    case TokenType::NOT: logger << "!" << std::endl; break;
                    default: logger << "?" << std::endl; break;
                }
            }
            printAST(unary->operand.get(), indent + 1, toConsole);
            break;
        }
        case ASTNodeType::IF_STATEMENT:
        {
            const IfNode* ifNode = static_cast<const IfNode*>(node);
            if (toConsole)
                std::cout << indentation << "If:" << std::endl;
            else
                logger << indentation << "If:" << std::endl;
            
            if (toConsole)
                std::cout << indentation << "  Condition:" << std::endl;
            else
                logger << indentation << "  Condition:" << std::endl;
            printAST(ifNode->condition.get(), indent + 2, toConsole);
            
            if (toConsole)
                std::cout << indentation << "  Then:" << std::endl;
            else
                logger << indentation << "  Then:" << std::endl;
            printAST(ifNode->thenBranch.get(), indent + 2, toConsole);
            
            if (ifNode->elseBranch)
            {
                if (toConsole)
                    std::cout << indentation << "  Else:" << std::endl;
                else
                    logger << indentation << "  Else:" << std::endl;
                printAST(ifNode->elseBranch.get(), indent + 2, toConsole);
            }
            break;
        }
        case ASTNodeType::WHILE_LOOP:
        {
            const WhileNode* whileNode = static_cast<const WhileNode*>(node);
            if (toConsole)
                std::cout << indentation << "While:" << std::endl;
            else
                logger << indentation << "While:" << std::endl;
            
            if (toConsole)
                std::cout << indentation << "  Condition:" << std::endl;
            else
                logger << indentation << "  Condition:" << std::endl;
            printAST(whileNode->condition.get(), indent + 2, toConsole);
            
            if (toConsole)
                std::cout << indentation << "  Body:" << std::endl;
            else
                logger << indentation << "  Body:" << std::endl;
            printAST(whileNode->body.get(), indent + 2, toConsole);
            break;
        }
        case ASTNodeType::FOR_LOOP:
        {
            const ForNode* forNode = static_cast<const ForNode*>(node);
            if (toConsole)
                std::cout << indentation << "For:" << std::endl;
            else
                logger << indentation << "For:" << std::endl;
            
            if (forNode->initialization)
            {
                if (toConsole)
                    std::cout << indentation << "  Init:" << std::endl;
                else
                    logger << indentation << "  Init:" << std::endl;
                printAST(forNode->initialization.get(), indent + 2, toConsole);
            }
            
            if (forNode->condition)
            {
                if (toConsole)
                    std::cout << indentation << "  Condition:" << std::endl;
                else
                    logger << indentation << "  Condition:" << std::endl;
                printAST(forNode->condition.get(), indent + 2, toConsole);
            }
            
            if (forNode->increment)
            {
                if (toConsole)
                    std::cout << indentation << "  Increment:" << std::endl;
                else
                    logger << indentation << "  Increment:" << std::endl;
                printAST(forNode->increment.get(), indent + 2, toConsole);
            }
            
            if (toConsole)
                std::cout << indentation << "  Body:" << std::endl;
            else
                logger << indentation << "  Body:" << std::endl;
            printAST(forNode->body.get(), indent + 2, toConsole);
            break;
        }
        case ASTNodeType::BLOCK:
        {
            const BlockNode* block = static_cast<const BlockNode*>(node);
            if (toConsole)
                std::cout << indentation << "Block:" << std::endl;
            else
                logger << indentation << "Block:" << std::endl;
            
            for (const auto& stmt : block->statements)
            {
                printAST(stmt.get(), indent + 1, toConsole);
            }
            break;
        }
    }
}
