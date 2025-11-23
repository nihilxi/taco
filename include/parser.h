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
    PRINT,
    IF_STATEMENT,
    WHILE_LOOP,
    FOR_LOOP,
    BLOCK,
    COMPARISON,
    LOGICAL_OP,
    UNARY_OP
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

// Comparison Node
struct ComparisonNode : public ASTNode
{
    TokenType op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    
    ComparisonNode(TokenType operation, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r)
        : op(operation), left(std::move(l)), right(std::move(r))
    {
        type = ASTNodeType::COMPARISON;
    }
};

// Logical Operation Node
struct LogicalOpNode : public ASTNode
{
    TokenType op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    
    LogicalOpNode(TokenType operation, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r)
        : op(operation), left(std::move(l)), right(std::move(r))
    {
        type = ASTNodeType::LOGICAL_OP;
    }
};

// Unary Operation Node (for NOT)
struct UnaryOpNode : public ASTNode
{
    TokenType op;
    std::unique_ptr<ASTNode> operand;
    
    UnaryOpNode(TokenType operation, std::unique_ptr<ASTNode> operand)
        : op(operation), operand(std::move(operand))
    {
        type = ASTNodeType::UNARY_OP;
    }
};

// Block Node (list of statements)
struct BlockNode : public ASTNode
{
    std::vector<std::unique_ptr<ASTNode>> statements;
    
    BlockNode()
    {
        type = ASTNodeType::BLOCK;
    }
};

// If Statement Node
struct IfNode : public ASTNode
{
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> thenBranch;
    std::unique_ptr<ASTNode> elseBranch; // Can be null
    
    IfNode(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> thenB, std::unique_ptr<ASTNode> elseB = nullptr)
        : condition(std::move(cond)), thenBranch(std::move(thenB)), elseBranch(std::move(elseB))
    {
        type = ASTNodeType::IF_STATEMENT;
    }
};

// While Loop Node
struct WhileNode : public ASTNode
{
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> body;
    
    WhileNode(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> b)
        : condition(std::move(cond)), body(std::move(b))
    {
        type = ASTNodeType::WHILE_LOOP;
    }
};

// For Loop Node
struct ForNode : public ASTNode
{
    std::unique_ptr<ASTNode> initialization;
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> increment;
    std::unique_ptr<ASTNode> body;
    
    ForNode(std::unique_ptr<ASTNode> init, std::unique_ptr<ASTNode> cond, 
            std::unique_ptr<ASTNode> inc, std::unique_ptr<ASTNode> b)
        : initialization(std::move(init)), condition(std::move(cond)), 
          increment(std::move(inc)), body(std::move(b))
    {
        type = ASTNodeType::FOR_LOOP;
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
    std::unique_ptr<ASTNode> parseComparison();
    std::unique_ptr<ASTNode> parseLogicalAnd();
    std::unique_ptr<ASTNode> parseLogicalOr();
    std::unique_ptr<ASTNode> parseUnary();
    std::unique_ptr<ASTNode> parseAssignment();
    std::unique_ptr<ASTNode> parsePrint();
    std::unique_ptr<ASTNode> parseIf();
    std::unique_ptr<ASTNode> parseWhile();
    std::unique_ptr<ASTNode> parseFor();
    std::unique_ptr<ASTNode> parseBlock();
    std::unique_ptr<ASTNode> parseStatement();

public:
    Parser(const std::vector<Token> &tokens);
    std::vector<std::unique_ptr<ASTNode>> parse();
    void printAST(const ASTNode* node, int indent = 0, bool toConsole = false);
};

#endif // PARSER_H
