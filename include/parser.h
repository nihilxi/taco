#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <memory>
#include "lexer.h"

// AST Node Types
enum class ASTNodeType
{
    ASSIGNMENT,
    BINARY_OP,
    IDENTIFIER,
    NUMBER,
    PRINT
};

// Base AST Node
struct ASTNode
{
    ASTNodeType type;
    virtual ~ASTNode() = default;
};

// Number Node
struct NumberNode : public ASTNode
{
    std::string value;
    NumberNode(const std::string &val) : value(val) 
    { 
        type = ASTNodeType::NUMBER; 
    }
};

// Identifier Node
struct IdentifierNode : public ASTNode
{
    std::string name;
    IdentifierNode(const std::string &n) : name(n) 
    { 
        type = ASTNodeType::IDENTIFIER; 
    }
};

// Binary Operation Node
struct BinaryOpNode : public ASTNode
{
    TokenType op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    
    BinaryOpNode(TokenType operation, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r)
        : op(operation), left(std::move(l)), right(std::move(r))
    {
        type = ASTNodeType::BINARY_OP;
    }
};

// Assignment Node
struct AssignmentNode : public ASTNode
{
    std::string identifier;
    std::unique_ptr<ASTNode> expression;
    
    AssignmentNode(const std::string &id, std::unique_ptr<ASTNode> expr)
        : identifier(id), expression(std::move(expr))
    {
        type = ASTNodeType::ASSIGNMENT;
    }
};

// Print Node
struct PrintNode : public ASTNode
{
    std::unique_ptr<ASTNode> expression;
    
    PrintNode(std::unique_ptr<ASTNode> expr)
        : expression(std::move(expr))
    {
        type = ASTNodeType::PRINT;
    }
};

// Parser Class
class Parser
{
private:
    std::vector<Token> tokens;
    size_t current;

    Token peek();
    Token advance();
    bool check(TokenType type);
    bool match(TokenType type);
    
    std::unique_ptr<ASTNode> parsePrimary();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<ASTNode> parsePrint();
    std::unique_ptr<ASTNode> parseStatement();

public:
    Parser(const std::vector<Token> &tokens);
    std::vector<std::unique_ptr<ASTNode>> parse();
    void printAST(const ASTNode* node, int indent = 0);
};

#endif // PARSER_H
