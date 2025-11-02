#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

enum class TokenType
{
    // Literals
    IDENTIFIER,
    NUMBER,
    
    // Arithmetic operators
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    
    // Comparison operators
    EQ,         // ==
    NEQ,        // !=
    LT,         // <
    LTE,        // <=
    GT,         // >
    GTE,        // >=
    
    // Logical operators
    AND,        // &&
    OR,         // ||
    NOT,        // !
    
    // Assignment
    ASSIGN,     // =
    
    // Keywords
    IF,
    ELSE,
    WHILE,
    FOR,
    RETURN,
    INT,
    FLOAT,
    DOUBLE,
    BOOL,
    VOID,
    TRUE,
    FALSE,
    
    // Delimiters
    LPAREN,     // (
    RPAREN,     // )
    LBRACE,     // {
    RBRACE,     // }
    LBRACKET,   // [
    RBRACKET,   // ]
    SEMICOLON,  // ;
    COMMA,      // ,
    
    END_OF_FILE
};

struct Token
{
    TokenType type;
    std::string value;
    int line;
    int column;
};

std::vector<Token> lexing(const std::string &source_code);

#endif // LEXER_H
