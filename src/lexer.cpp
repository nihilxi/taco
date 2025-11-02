#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <string>
#include <vector>
#include "../include/lexer.h"

std::vector<Token> lexing(const std::string &source_code)
{
    std::vector<Token> tokens;
    int line = 1;
    int column = 1;

    for (size_t i = 0; i < source_code.size(); i++)
    {
        char symbol = source_code[i];
        
        // Skip whitespace
        if (isspace(symbol))
        {
            if (symbol == '\n')
            {
                line++;
                column = 1;
            }
            else
            {
                column++;
            }
            continue;
        }

        // Skip line comments
        if (symbol == '/' && i + 1 < source_code.size() && source_code[i + 1] == '/')
        {
            while (i < source_code.size() && source_code[i] != '\n')
            {
                i++;
            }
            line++;
            column = 1;
            continue;
        }

        // Skip block comments
        if (symbol == '/' && i + 1 < source_code.size() && source_code[i + 1] == '*')
        {
            i += 2;
            column += 2;
            while (i + 1 < source_code.size())
            {
                if (source_code[i] == '*' && source_code[i + 1] == '/')
                {
                    i += 2;
                    column += 2;
                    break;
                }
                if (source_code[i] == '\n')
                {
                    line++;
                    column = 1;
                }
                else
                {
                    column++;
                }
                i++;
            }
            continue;
        }

        // Identifier or keyword
        if (isalpha(symbol) || symbol == '_')
        {
            std::string identifier;
            int start_col = column;
            
            while (i < source_code.size() && (isalnum(source_code[i]) || source_code[i] == '_'))
            {
                identifier += source_code[i];
                i++;
                column++;
            }
            i--;
            
            // Check for keywords
            TokenType type = TokenType::IDENTIFIER;
            if (identifier == "if") type = TokenType::IF;
            else if (identifier == "else") type = TokenType::ELSE;
            else if (identifier == "while") type = TokenType::WHILE;
            else if (identifier == "for") type = TokenType::FOR;
            else if (identifier == "return") type = TokenType::RETURN;
            else if (identifier == "int") type = TokenType::INT;
            else if (identifier == "float") type = TokenType::FLOAT;
            else if (identifier == "double") type = TokenType::DOUBLE;
            else if (identifier == "bool") type = TokenType::BOOL;
            else if (identifier == "void") type = TokenType::VOID;
            else if (identifier == "true") type = TokenType::TRUE;
            else if (identifier == "false") type = TokenType::FALSE;
            
            tokens.push_back({type, identifier, line, start_col});
        }
        // Number
        else if (isdigit(symbol))
        {
            std::string number;
            int start_col = column;
            
            while (i < source_code.size() && (isdigit(source_code[i]) || source_code[i] == '.'))
            {
                number += source_code[i];
                i++;
                column++;
            }
            i--;
            
            tokens.push_back({TokenType::NUMBER, number, line, start_col});
        }
        // Operators and delimiters
        else if (symbol == '+')
        {
            tokens.push_back({TokenType::ADD, "+", line, column});
            column++;
        }
        else if (symbol == '-')
        {
            tokens.push_back({TokenType::SUB, "-", line, column});
            column++;
        }
        else if (symbol == '*')
        {
            tokens.push_back({TokenType::MUL, "*", line, column});
            column++;
        }
        else if (symbol == '/')
        {
            tokens.push_back({TokenType::DIV, "/", line, column});
            column++;
        }
        else if (symbol == '%')
        {
            tokens.push_back({TokenType::MOD, "%", line, column});
            column++;
        }
        else if (symbol == '=')
        {
            if (i + 1 < source_code.size() && source_code[i + 1] == '=')
            {
                tokens.push_back({TokenType::EQ, "==", line, column});
                i++;
                column += 2;
            }
            else
            {
                tokens.push_back({TokenType::ASSIGN, "=", line, column});
                column++;
            }
        }
        else if (symbol == '!')
        {
            if (i + 1 < source_code.size() && source_code[i + 1] == '=')
            {
                tokens.push_back({TokenType::NEQ, "!=", line, column});
                i++;
                column += 2;
            }
            else
            {
                tokens.push_back({TokenType::NOT, "!", line, column});
                column++;
            }
        }
        else if (symbol == '<')
        {
            if (i + 1 < source_code.size() && source_code[i + 1] == '=')
            {
                tokens.push_back({TokenType::LTE, "<=", line, column});
                i++;
                column += 2;
            }
            else
            {
                tokens.push_back({TokenType::LT, "<", line, column});
                column++;
            }
        }
        else if (symbol == '>')
        {
            if (i + 1 < source_code.size() && source_code[i + 1] == '=')
            {
                tokens.push_back({TokenType::GTE, ">=", line, column});
                i++;
                column += 2;
            }
            else
            {
                tokens.push_back({TokenType::GT, ">", line, column});
                column++;
            }
        }
        else if (symbol == '&' && i + 1 < source_code.size() && source_code[i + 1] == '&')
        {
            tokens.push_back({TokenType::AND, "&&", line, column});
            i++;
            column += 2;
        }
        else if (symbol == '|' && i + 1 < source_code.size() && source_code[i + 1] == '|')
        {
            tokens.push_back({TokenType::OR, "||", line, column});
            i++;
            column += 2;
        }
        else if (symbol == '(')
        {
            tokens.push_back({TokenType::LPAREN, "(", line, column});
            column++;
        }
        else if (symbol == ')')
        {
            tokens.push_back({TokenType::RPAREN, ")", line, column});
            column++;
        }
        else if (symbol == '{')
        {
            tokens.push_back({TokenType::LBRACE, "{", line, column});
            column++;
        }
        else if (symbol == '}')
        {
            tokens.push_back({TokenType::RBRACE, "}", line, column});
            column++;
        }
        else if (symbol == '[')
        {
            tokens.push_back({TokenType::LBRACKET, "[", line, column});
            column++;
        }
        else if (symbol == ']')
        {
            tokens.push_back({TokenType::RBRACKET, "]", line, column});
            column++;
        }
        else if (symbol == ';')
        {
            tokens.push_back({TokenType::SEMICOLON, ";", line, column});
            column++;
        }
        else if (symbol == ',')
        {
            tokens.push_back({TokenType::COMMA, ",", line, column});
            column++;
        }
        else
        {
            column++;
        }
    }
    
    tokens.push_back({TokenType::END_OF_FILE, "", line, column});
    
    return tokens;
}