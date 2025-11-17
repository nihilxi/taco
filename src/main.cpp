#include <iostream>
#include <fstream>
#include <string>
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/tac.h"
#include "../include/energy.h"
#include "../include/codegen.h"
#include "../include/logger.h"

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
    std::cout << "  -o <file>         Output executable file (default: output)\n";
    std::cout << "  --c-only          Generate only C code without compiling\n";
    std::cout << "  --log <file>      Output log file (default: compilation.log)\n";
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
    bool cOnly = false;
    std::string outputFile = "output";
    std::string logFile = "compilation.log";
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
        else if (arg == "--c-only")
        {
            cOnly = true;
        }
        else if (arg == "-o" && i + 1 < argc)
        {
            outputFile = argv[++i];
        }
        else if (arg == "--log" && i + 1 < argc)
        {
            logFile = argv[++i];
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

    // Open log file
    if (!logger.open(logFile))
    {
        std::cerr << "Error: Could not open log file: " << logFile << std::endl;
        return 1;
    }
    std::cout << "Logging to: " << logFile << std::endl;

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

    logger << "Compiling: " << inputFile << "\n\n";

    // 1. Lexical Analysis
    logger.startTimer();
    std::vector<Token> tokens = lexing(sourceCode);
    logger.endTimer("Lexical Analysis");
    
    if (showTokens)
    {
        logger << "=== TOKENS ===" << std::endl;
        for (const auto &token : tokens)
        {
            if (token.type == TokenType::END_OF_FILE)
                break;
            logger << "Line " << token.line << ":" << token.column 
                   << " - " << token.value << std::endl;
        }
        logger << std::endl;
    }

    // 2. Syntax Analysis (Parsing)
    logger.startTimer();
    Parser parser(tokens);
    auto ast = parser.parse();
    logger.endTimer("Syntax Analysis (Parsing)");
    
    // Always print AST to logs
    logger << "=== ABSTRACT SYNTAX TREE ===" << std::endl;
    for (const auto &statement : ast)
    {
        parser.printAST(statement.get(), 0, false);
    }
    logger << std::endl;
    
    // Also print to console if flag is set
    if (showAST)
    {
        std::cout << "=== ABSTRACT SYNTAX TREE ===" << std::endl;
        for (const auto &statement : ast)
        {
            parser.printAST(statement.get(), 0, true);
        }
        std::cout << std::endl;
    }

    // 3. TAC Generation
    logger.startTimer();
    TACGenerator tacGen;
    std::vector<TACInstruction> tac = tacGen.generate(ast);
    logger.endTimer("TAC Generation");
    
    // Always print TAC to logs
    tacGen.printTAC(tac, false);
    logger << std::endl;
    
    // Also print to console if flag is set
    if (showTAC)
    {
        tacGen.printTAC(tac, true);
        std::cout << std::endl;
    }

    // 4. Energy Analysis
    logger.startTimer();
    EnergyModel energyModel;
    
    if (showEnergyTable)
    {
        energyModel.printEnergyTable();
        logger << std::endl;
    }
    
    if (showEnergy)
    {
        energyModel.printEnergyReport(tac);
        logger << std::endl;
    }
    logger.endTimer("Energy Analysis");

    // 5. Code Generation (C)
    logger.startTimer();
    CCodeGenerator codeGen;
    std::string cCode = codeGen.generate(tac);
    
    std::string cFilename = outputFile + ".c";
    codeGen.writeToFile(cCode, cFilename);
    logger.endTimer("C Code Generation");

    // 6. Compile to executable (unless --c-only flag is set)
    if (!cOnly)
    {
        logger.startTimer();
        if (codeGen.compileToExecutable(cFilename, outputFile))
        {
            logger.endTimer("C to Executable Compilation");
            logger << "\nCompilation successful!\n";
            logger << "Executable: " << outputFile << std::endl;
        }
        else
        {
            logger.endTimer("C to Executable Compilation");
            logger << "\nExecutable creation failed!\n";
            logger << "C code is available in: " << cFilename << std::endl;
            return 1;
        }
    }
    else
    {
        logger << "\nC code generation successful!\n";
        logger << "Output: " << cFilename << std::endl;
    }
    
    // Print summary
    logger << "\n=== COMPILATION SUMMARY ===" << std::endl;
    logger << "Tokens: " << tokens.size() << std::endl;
    logger << "AST nodes: " << ast.size() << std::endl;
    logger << "TAC instructions: " << tac.size() << std::endl;
    logger << "Estimated energy cost: " << energyModel.calculateProgramEnergy(tac) 
           << " units" << std::endl;
    
    // Print timing report
    logger.printTimingReport();

    logger.close();
    std::cout << "\nCompilation complete. Log saved to: " << logFile << std::endl;
    
    return 0;
}
