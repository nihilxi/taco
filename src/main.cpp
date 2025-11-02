#include <iostream>
#include <fstream>
#include <string>
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/tac.h"
#include "../include/energy.h"
#include "../include/codegen.h"

void printUsage(const char* program)
{
    std::cout << "TACO Compiler - Three-Address Code Compiler\n";
    std::cout << "Usage: " << program << " [options] <source_file.taco>\n\n";
    std::cout << "Options:\n";
    std::cout << "  --tokens          Show token stream\n";
    std::cout << "  --ast             Show abstract syntax tree\n";
    std::cout << "  --tac             Show three-address code\n";
    std::cout << "  --energy          Show energy consumption report\n";
    std::cout << "  --energy-table    Show energy cost table\n";
    std::cout << "  -o <file>         Output C file (default: output.c)\n";
    std::cout << "  --help            Show this help message\n";
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printUsage(argv[0]);
        return 1;
    }

    // Parse command line arguments
    bool showTokens = false;
    bool showAST = false;
    bool showTAC = false;
    bool showEnergy = false;
    bool showEnergyTable = false;
    std::string outputFile = "output.c";
    std::string inputFile;

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        
        if (arg == "--help")
        {
            printUsage(argv[0]);
            return 0;
        }
        else if (arg == "--tokens")
        {
            showTokens = true;
        }
        else if (arg == "--ast")
        {
            showAST = true;
        }
        else if (arg == "--tac")
        {
            showTAC = true;
        }
        else if (arg == "--energy")
        {
            showEnergy = true;
        }
        else if (arg == "--energy-table")
        {
            showEnergyTable = true;
        }
        else if (arg == "-o" && i + 1 < argc)
        {
            outputFile = argv[++i];
        }
        else if (arg[0] != '-')
        {
            inputFile = arg;
        }
    }

    if (inputFile.empty())
    {
        std::cerr << "Error: No input file specified\n";
        printUsage(argv[0]);
        return 1;
    }

    // Check file extension
    size_t dotPos = inputFile.find_last_of(".");
    if (dotPos == std::string::npos || inputFile.substr(dotPos + 1) != "taco")
    {
        std::cerr << "Error: Input file must have .taco extension\n";
        return 1;
    }

    // Read source file
    std::ifstream sourceFile(inputFile);
    if (!sourceFile.is_open())
    {
        std::cerr << "Error: Could not open file: " << inputFile << std::endl;
        return 1;
    }

    std::string sourceCode;
    std::string line;
    while (std::getline(sourceFile, line))
    {
        sourceCode += line + "\n";
    }
    sourceFile.close();

    std::cout << "Compiling: " << inputFile << "\n\n";

    // 1. Lexical Analysis
    std::vector<Token> tokens = lexing(sourceCode);
    
    if (showTokens)
    {
        std::cout << "=== TOKENS ===" << std::endl;
        for (const auto &token : tokens)
        {
            if (token.type == TokenType::END_OF_FILE)
                break;
            std::cout << "Line " << token.line << ":" << token.column 
                      << " - " << token.value << std::endl;
        }
        std::cout << std::endl;
    }

    // 2. Syntax Analysis (Parsing)
    Parser parser(tokens);
    auto ast = parser.parse();
    
    if (showAST)
    {
        std::cout << "=== ABSTRACT SYNTAX TREE ===" << std::endl;
        for (const auto &statement : ast)
        {
            parser.printAST(statement.get());
        }
        std::cout << std::endl;
    }

    // 3. TAC Generation
    TACGenerator tacGen;
    std::vector<TACInstruction> tac = tacGen.generate(ast);
    
    if (showTAC)
    {
        tacGen.printTAC(tac);
        std::cout << std::endl;
    }

    // 4. Energy Analysis
    EnergyModel energyModel;
    
    if (showEnergyTable)
    {
        energyModel.printEnergyTable();
        std::cout << std::endl;
    }
    
    if (showEnergy)
    {
        energyModel.printEnergyReport(tac);
        std::cout << std::endl;
    }

    // 5. Code Generation (C)
    CCodeGenerator codeGen;
    std::string cCode = codeGen.generate(tac);
    codeGen.writeToFile(cCode, outputFile);

    std::cout << "\nCompilation successful!\n";
    std::cout << "Output: " << outputFile << std::endl;
    
    // Print summary
    std::cout << "\n=== COMPILATION SUMMARY ===" << std::endl;
    std::cout << "Tokens: " << tokens.size() << std::endl;
    std::cout << "AST nodes: " << ast.size() << std::endl;
    std::cout << "TAC instructions: " << tac.size() << std::endl;
    std::cout << "Estimated energy cost: " << energyModel.calculateProgramEnergy(tac) 
              << " units" << std::endl;

    return 0;
}
