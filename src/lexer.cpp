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
            if (identifier == "print")
            {
                tokens.push_back({TokenType::PRINT, identifier, line, start_col});
            }
            else
            {
                // All other identifiers remain as IDENTIFIER
                tokens.push_back({TokenType::IDENTIFIER, identifier, line, start_col});
            }
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
        else if (symbol == '=')
        {
            tokens.push_back({TokenType::ASSIGN, "=", line, column});
            column++;
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
        else
        {
            column++;
        }
    }
    
    tokens.push_back({TokenType::END_OF_FILE, "", line, column});
    
    return tokens;
}