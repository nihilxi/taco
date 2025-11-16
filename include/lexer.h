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
    
    // Assignment
    ASSIGN,     // =
    
    // Keywords
    PRINT,      // print
    
    // Delimiters
    LPAREN,     // (
    RPAREN,     // )
    
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
