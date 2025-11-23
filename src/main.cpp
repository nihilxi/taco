#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/tac.h"
#include "../include/codegen.h"
#include "../include/logger.h"

void printUsage(const char *program)
{
    std::cout << "TACO Compiler - Three-Address Code Compiler\n";
    std::cout << "Usage: " << program << " [options] <source_file.taco>\n\n";
    std::cout << "Options:\n";
    std::cout << "  -o <file>         Output executable file (default: output)\n";
    std::cout << "  --gen-c           Generate C code file (required for --c-only)\n";
    std::cout << "  --c-only          Generate only C code without compiling (implies --gen-c)\n";
    std::cout << "  --log <options>   Enable logging with specified components (comma-separated)\n";
    std::cout << "                    Options: tokens, ast, tac, timing, profile, all\n";
    std::cout << "                    Example: --log tokens,ast or --log all\n";
    std::cout << "                    Log file: compilation_DDMMYYYY_HHMMSS.log\n";
    std::cout << "  --help            Show this help message\n\n";
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printUsage(argv[0]);
        return 1;
    }

    // Parse command line arguments
    bool logTokens = false;
    bool logAST = false;
    bool logTAC = false;
    bool logTiming = false;
    bool cOnly = false;
    bool generateC = false;
    bool enableLogging = false;
    bool logProfiling = false;
    std::string outputFile = "output";
    std::string logFile = "";
    std::string inputFile;

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];

        if (arg == "--help")
        {
            printUsage(argv[0]);
            return 0;
        }
        else if (arg == "--gen-c")
        {
            generateC = true;
        }
        else if (arg == "--c-only")
        {
            cOnly = true;
            generateC = true; // --c-only implies --gen-c
        }
        else if (arg == "-o" && i + 1 < argc)
        {
            outputFile = argv[++i];
        }
        else if (arg == "--log" && i + 1 < argc)
        {
            enableLogging = true;
            std::string logOptions = argv[++i];
            
            // Parse comma-separated log options
            std::istringstream iss(logOptions);
            std::string option;
            while (std::getline(iss, option, ','))
            {
                if (option == "all")
                {
                    logTokens = true;
                    logAST = true;
                    logTAC = true;
                    logTiming = true;
                    logProfiling = true;
                }
                else if (option == "tokens")
                {
                    logTokens = true;
                }
                else if (option == "ast")
                {
                    logAST = true;
                }
                else if (option == "tac")
                {
                    logTAC = true;
                }
                else if (option == "timing")
                {
                    logTiming = true;
                }
                else if (option == "profile")
                {
                    logProfiling = true;
                }
                else
                {
                    std::cerr << "Warning: Unknown log option '" << option << "' (valid: tokens, ast, tac, timing, profile, all)\n";
                }
            }
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

    // Open log file if logging is enabled
    if (enableLogging)
    {
        // Generate log filename with timestamp if not specified
        if (logFile.empty())
        {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            std::tm *tm_time = std::localtime(&time);

            std::ostringstream oss;
            oss << "compilation_"
                << std::setfill('0') << std::setw(2) << tm_time->tm_mday
                << std::setfill('0') << std::setw(2) << (tm_time->tm_mon + 1)
                << std::setfill('0') << std::setw(4) << (tm_time->tm_year + 1900)
                << "_"
                << std::setfill('0') << std::setw(2) << tm_time->tm_hour
                << std::setfill('0') << std::setw(2) << tm_time->tm_min
                << std::setfill('0') << std::setw(2) << tm_time->tm_sec
                << ".log";
            logFile = oss.str();
        }

        if (!logger.open(logFile))
        {
            std::cerr << "Error: Could not open log file: " << logFile << std::endl;
            return 1;
        }
        std::cout << "Logging to: " << logFile << std::endl;
        
        // Enable profiling if requested
        if (logProfiling)
        {
            logger.enableProfiling(true);
        }
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

    if (enableLogging)
    {
        logger << "Compiling: " << inputFile << "\n\n";
    }

    // 1. Lexical Analysis
    logger.startTimer();
    if (logProfiling)
        logger.startProfiling();
    
    std::vector<Token> tokens = lexing(sourceCode);
    
    if (logProfiling)
        logger.endProfiling("Lexical Analysis");
    logger.endTimer("Lexical Analysis");

    if (logTokens && enableLogging)
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
    if (logProfiling)
        logger.startProfiling();
    
    Parser parser(tokens);
    auto ast = parser.parse();
    
    if (logProfiling)
        logger.endProfiling("Syntax Analysis (Parsing)");
    logger.endTimer("Syntax Analysis (Parsing)");

    // Print AST to logs if requested
    if (logAST && enableLogging)
    {
        logger << "=== ABSTRACT SYNTAX TREE ===" << std::endl;
        for (const auto &statement : ast)
        {
            parser.printAST(statement.get(), 0, false);
        }
        logger << std::endl;
    }

    // 3. TAC Generation
    logger.startTimer();
    if (logProfiling)
        logger.startProfiling();
    
    TACGenerator tacGen;
    std::vector<TACInstruction> tac = tacGen.generate(ast);
    
    if (logProfiling)
        logger.endProfiling("TAC Generation");
    logger.endTimer("TAC Generation");

    // Print TAC to logs if requested
    if (logTAC && enableLogging)
    {
        tacGen.printTAC(tac, false);
        logger << std::endl;
    }

    // 4. Code Generation (C) - only if --gen-c or --c-only flag is set
    std::string cFilename;
    if (generateC)
    {
        logger.startTimer();
        if (logProfiling)
            logger.startProfiling();
        
        CCodeGenerator codeGen;
        std::string cCode = codeGen.generate(tac);

        cFilename = outputFile + ".c";
        codeGen.writeToFile(cCode, cFilename);
        
        if (logProfiling)
            logger.endProfiling("C Code Generation");
        logger.endTimer("C Code Generation");

        // Compile to executable (unless --c-only flag is set)
        if (!cOnly)
        {
            logger.startTimer();
            if (logProfiling)
                logger.startProfiling();
            
            if (codeGen.compileToExecutable(cFilename, outputFile))
            {
                if (logProfiling)
                    logger.endProfiling("C to Executable Compilation");
                logger.endTimer("C to Executable Compilation");
                if (enableLogging)
                {
                    logger << "Compilation successful!\n";
                    logger << "Executable: " << outputFile << std::endl;
                }
                std::cout << "Executable created: " << outputFile << std::endl;
            }
            else
            {
                if (logProfiling)
                    logger.endProfiling("C to Executable Compilation");
                logger.endTimer("C to Executable Compilation");
                if (enableLogging)
                {
                    logger << "\nExecutable creation failed!\n";
                    logger << "C code is available in: " << cFilename << std::endl;
                }
                std::cerr << "Error: Failed to compile C code to executable" << std::endl;
                return 1;
            }
        }
        else
        {
            if (enableLogging)
            {
                logger << "\nC code generation successful!\n";
                logger << "Output: " << cFilename << std::endl;
            }
            std::cout << "C code generated: " << cFilename << std::endl;
        }
    }
    else
    {
        // Default behavior: compile directly to executable without saving C file
        logger.startTimer();
        if (logProfiling)
            logger.startProfiling();
        
        CCodeGenerator codeGen;
        std::string cCode = codeGen.generate(tac);

        // Create temporary C file
        cFilename = outputFile + "_temp.c";
        codeGen.writeToFile(cCode, cFilename);
        
        if (logProfiling)
            logger.endProfiling("C Code Generation");
        logger.endTimer("C Code Generation");

        logger.startTimer();
        if (logProfiling)
            logger.startProfiling();
        
        if (codeGen.compileToExecutable(cFilename, outputFile))
        {
            // Remove temporary C file after successful compilation
            std::remove(cFilename.c_str());

            if (logProfiling)
                logger.endProfiling("C to Executable Compilation");
            logger.endTimer("C to Executable Compilation");
            if (enableLogging)
            {
                logger << "Compilation successful!\n";
                logger << "Executable: " << outputFile << std::endl;
            }
            std::cout << "Compilation successful! Executable: " << outputFile << std::endl;
        }
        else
        {
            if (logProfiling)
                logger.endProfiling("C to Executable Compilation");
            logger.endTimer("C to Executable Compilation");
            if (enableLogging)
            {
                logger << "\nExecutable creation failed!\n";
            }
            std::cerr << "Error: Compilation failed" << std::endl;
            return 1;
        }
    }

    // Print summary - always show in console
    std::cout << "\n=== COMPILATION SUMMARY ===" << std::endl;
    std::cout << "Tokens: " << tokens.size() << std::endl;
    std::cout << "AST nodes: " << ast.size() << std::endl;
    std::cout << "TAC instructions: " << tac.size() << std::endl;
    
    // Print timing report - always show in console
    logger.printTimingReportToConsole();
    
    // Print profiling report to console if enabled
    if (logProfiling)
    {
        logger.printProfilingReportToConsole();
    }
    
    // Print summary and reports to log file if logging enabled
    if (enableLogging)
    {
        logger << "\n=== COMPILATION SUMMARY ===" << std::endl;
        logger << "Tokens: " << tokens.size() << std::endl;
        logger << "AST nodes: " << ast.size() << std::endl;
        logger << "TAC instructions: " << tac.size() << std::endl;

        // Print timing report to log if timing flag is set
        if (logTiming)
        {
            logger.printTimingReportToLog();
        }
        
        // Print profiling report to log if enabled
        if (logProfiling)
        {
            logger.printProfilingReportToLog();
        }

        logger.close();
        std::cout << "\nCompilation complete. Log saved to: " << logFile << std::endl;
    }

    return 0;
}
