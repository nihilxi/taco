#include <iostream>
#include <fstream>
#include "../include/parser.h"
#include "../include/lexer.h"

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

// Parse primary expressions (numbers and identifiers)
std::unique_ptr<ASTNode> Parser::parsePrimary()
{
    Token token = peek();
    
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
    
    std::cerr << "Error: Expected number or identifier at line " << token.line << std::endl;
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
            std::cerr << "Error: Expected '=' after identifier at line " << token.line << std::endl;
            return nullptr;
        }
    }
    
    std::cerr << "Error: Expected identifier at line " << token.line << std::endl;
    return nullptr;
}

// Parse the entire program
std::vector<std::unique_ptr<ASTNode>> Parser::parse()
{
    std::vector<std::unique_ptr<ASTNode>> statements;
    
    while (!check(TokenType::END_OF_FILE))
    {
        auto statement = parseAssignment();
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
void Parser::printAST(const ASTNode* node, int indent)
{
    if (!node)
        return;
        
    std::string indentation(indent * 2, ' ');
    
    switch (node->type)
    {
        case ASTNodeType::NUMBER:
        {
            const NumberNode* num = static_cast<const NumberNode*>(node);
            std::cout << indentation << "Number: " << num->value << std::endl;
            break;
        }
        case ASTNodeType::IDENTIFIER:
        {
            const IdentifierNode* id = static_cast<const IdentifierNode*>(node);
            std::cout << indentation << "Identifier: " << id->name << std::endl;
            break;
        }
        case ASTNodeType::BINARY_OP:
        {
            const BinaryOpNode* binop = static_cast<const BinaryOpNode*>(node);
            std::cout << indentation << "BinaryOp: ";
            switch (binop->op)
            {
                case TokenType::ADD: std::cout << "+" << std::endl; break;
                case TokenType::SUB: std::cout << "-" << std::endl; break;
                case TokenType::MUL: std::cout << "*" << std::endl; break;
                case TokenType::DIV: std::cout << "/" << std::endl; break;
                default: std::cout << "?" << std::endl; break;
            }
            printAST(binop->left.get(), indent + 1);
            printAST(binop->right.get(), indent + 1);
            break;
        }
        case ASTNodeType::ASSIGNMENT:
        {
            const AssignmentNode* assign = static_cast<const AssignmentNode*>(node);
            std::cout << indentation << "Assignment: " << assign->identifier << std::endl;
            printAST(assign->expression.get(), indent + 1);
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <source_file.taco>" << std::endl;
        return 1;
    }

    std::string fn = argv[1], text;

    if (fn.substr(fn.find_last_of(".") + 1) == "taco")
    {
        std::ifstream source_code(argv[1]);

        if (source_code.is_open())
        {
            std::string line;
            while (std::getline(source_code, line))
            {
                text += line + "\n";
            }
            source_code.close();

            // Lexing phase
            std::vector<Token> tokens = lexing(text);
            
            std::cout << "=== TOKENS ===" << std::endl;
            for (const auto &token : tokens)
            {
                std::cout << "Type: ";
                switch (token.type)
                {
                    case TokenType::IDENTIFIER: std::cout << "IDENTIFIER"; break;
                    case TokenType::NUMBER: std::cout << "NUMBER"; break;
                    case TokenType::ADD: std::cout << "ADD"; break;
                    case TokenType::SUB: std::cout << "SUB"; break;
                    case TokenType::MUL: std::cout << "MUL"; break;
                    case TokenType::DIV: std::cout << "DIV"; break;
                    case TokenType::ASSIGN: std::cout << "ASSIGN"; break;
                    case TokenType::END_OF_FILE: std::cout << "EOF"; break;
                }
                std::cout << ", Value: '" << token.value << "', Line: " << token.line 
                          << ", Column: " << token.column << std::endl;
            }
            
            // Parsing phase
            std::cout << "\n=== ABSTRACT SYNTAX TREE ===" << std::endl;
            Parser parser(tokens);
            auto ast = parser.parse();
            
            for (const auto &statement : ast)
            {
                parser.printAST(statement.get());
                std::cout << std::endl;
            }
        }
        else
        {
            std::cerr << "Error: Could not open file" << std::endl;
            return 1;
        }
    }
    else
    {
        std::cerr << "Error: Incorrect source file extension (expected .taco)" << std::endl;
        return 1;
    }

    return 0;
}
