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

        if (isalpha(symbol))
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
            
            tokens.push_back({TokenType::IDENTIFIER, identifier, line, start_col});
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
        else
        {
            column++;
        }
    }
    
    tokens.push_back({TokenType::END_OF_FILE, "", line, column});
    
    return tokens;
}

#ifndef LIB_BUILD
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
#endif // LIB_BUILD