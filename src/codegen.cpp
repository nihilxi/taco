#include <iostream>
#include <fstream>
#include <sstream>
#include "../include/codegen.h"

CCodeGenerator::CCodeGenerator() {}

// Collect all variables and temporaries from TAC
void CCodeGenerator::collectVariables(const std::vector<TACInstruction>& tac)
{
    variables.clear();
    temporaries.clear();
    
    for (const auto& instr : tac)
    {
        // Collect result variable
        if (!instr.result.empty())
        {
            if (instr.result[0] == 't')
                temporaries.insert(instr.result);
            else
                variables.insert(instr.result);
        }
        
        // Collect argument variables
        if (!instr.arg1.empty() && !isdigit(instr.arg1[0]))
        {
            if (instr.arg1[0] == 't')
                temporaries.insert(instr.arg1);
            else
                variables.insert(instr.arg1);
        }
        
        if (!instr.arg2.empty() && !isdigit(instr.arg2[0]))
        {
            if (instr.arg2[0] == 't')
                temporaries.insert(instr.arg2);
            else
                variables.insert(instr.arg2);
        }
    }
}

// Generate variable declarations
std::string CCodeGenerator::generateVariableDeclarations()
{
    std::ostringstream oss;
    
    // Declare user variables
    if (!variables.empty())
    {
        oss << "    // User variables\n";
        for (const auto& var : variables)
        {
            oss << "    double " << var << " = 0.0;\n";
        }
        oss << "\n";
    }
    
    // Declare temporaries
    if (!temporaries.empty())
    {
        oss << "    // Temporary variables\n";
        for (const auto& temp : temporaries)
        {
            oss << "    double " << temp << " = 0.0;\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

// Generate C code for a single TAC instruction
std::string CCodeGenerator::generateInstruction(const TACInstruction& instr)
{
    std::ostringstream oss;
    
    switch (instr.opcode)
    {
        case TACOpcode::ADD:
            oss << "    " << instr.result << " = " << instr.arg1 << " + " << instr.arg2 << ";";
            break;
        case TACOpcode::SUB:
            oss << "    " << instr.result << " = " << instr.arg1 << " - " << instr.arg2 << ";";
            break;
        case TACOpcode::MUL:
            oss << "    " << instr.result << " = " << instr.arg1 << " * " << instr.arg2 << ";";
            break;
        case TACOpcode::DIV:
            oss << "    " << instr.result << " = " << instr.arg1 << " / " << instr.arg2 << ";";
            break;
        case TACOpcode::ASSIGN:
            oss << "    " << instr.result << " = " << instr.arg1 << ";";
            break;
        case TACOpcode::PRINT:
            oss << "    printf(\"%g\\n\", " << instr.arg1 << ");";
            break;
        default:
            oss << "    // Unsupported instruction";
            break;
    }
    
    return oss.str();
}

// Generate complete C program from TAC
std::string CCodeGenerator::generate(const std::vector<TACInstruction>& tac)
{
    collectVariables(tac);
    
    std::ostringstream oss;
    
    // Generate C header
    oss << "// Generated C code from TACO compiler\n";
    oss << "#include <stdio.h>\n";
    oss << "#include <math.h>\n\n";
    
    // Generate main function
    oss << "int main() {\n";
    
    // Variable declarations
    oss << generateVariableDeclarations();
    
    // Generate instructions
    oss << "    // Program code\n";
    for (const auto& instr : tac)
    {
        std::string line = generateInstruction(instr);
        if (!line.empty())
        {
            oss << line << "\n";
        }
    }
    
    // End main function
    oss << "\n    return 0;\n";
    oss << "}\n";
    
    return oss.str();
}

// Write generated code to file
void CCodeGenerator::writeToFile(const std::string& code, const std::string& filename)
{
    std::ofstream outFile(filename);
    
    if (outFile.is_open())
    {
        outFile << code;
        outFile.close();
        std::cout << "C code written to: " << filename << std::endl;
    }
    else
    {
        std::cerr << "Error: Could not open file " << filename << " for writing" << std::endl;
    }
}

// Compile C code to executable using GCC
bool CCodeGenerator::compileToExecutable(const std::string& cFilename, const std::string& outputExecutable)
{
    std::string command = "gcc -o " + outputExecutable + " " + cFilename + " -lm 2>&1";
    
    std::cout << "Compiling C code to executable..." << std::endl;
    std::cout << "Command: " << command << std::endl;
    
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe)
    {
        std::cerr << "Error: Could not execute GCC compiler" << std::endl;
        return false;
    }
    
    // Read compiler output
    char buffer[256];
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        result += buffer;
    }
    
    int returnCode = pclose(pipe);
    
    if (returnCode != 0)
    {
        std::cerr << "GCC compilation failed:" << std::endl;
        std::cerr << result << std::endl;
        return false;
    }
    
    if (!result.empty())
    {
        std::cout << "Compiler warnings/notes:" << std::endl;
        std::cout << result << std::endl;
    }
    
    std::cout << "Executable created: " << outputExecutable << std::endl;
    return true;
}
