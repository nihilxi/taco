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
        if (!instr.result.empty() && instr.opcode != TACOpcode::LABEL && instr.opcode != TACOpcode::GOTO)
        {
            if (instr.result[0] == 't')
                temporaries.insert(instr.result);
            else
                variables.insert(instr.result);
        }
        
        // Collect argument variables
        if (!instr.arg1.empty() && !isdigit(instr.arg1[0]) && instr.arg1 != "true" && instr.arg1 != "false")
        {
            if (instr.arg1[0] == 't')
                temporaries.insert(instr.arg1);
            else if (instr.opcode != TACOpcode::CALL && instr.opcode != TACOpcode::GOTO && 
                     instr.opcode != TACOpcode::IF_FALSE && instr.opcode != TACOpcode::IF_TRUE)
                variables.insert(instr.arg1);
        }
        
        if (!instr.arg2.empty() && !isdigit(instr.arg2[0]) && instr.arg2 != "true" && instr.arg2 != "false")
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
        case TACOpcode::MOD:
            oss << "    " << instr.result << " = (int)" << instr.arg1 << " % (int)" << instr.arg2 << ";";
            break;
        case TACOpcode::NEG:
            oss << "    " << instr.result << " = -" << instr.arg1 << ";";
            break;
        case TACOpcode::AND:
            oss << "    " << instr.result << " = " << instr.arg1 << " && " << instr.arg2 << ";";
            break;
        case TACOpcode::OR:
            oss << "    " << instr.result << " = " << instr.arg1 << " || " << instr.arg2 << ";";
            break;
        case TACOpcode::NOT:
            oss << "    " << instr.result << " = !" << instr.arg1 << ";";
            break;
        case TACOpcode::EQ:
            oss << "    " << instr.result << " = " << instr.arg1 << " == " << instr.arg2 << ";";
            break;
        case TACOpcode::NEQ:
            oss << "    " << instr.result << " = " << instr.arg1 << " != " << instr.arg2 << ";";
            break;
        case TACOpcode::LT:
            oss << "    " << instr.result << " = " << instr.arg1 << " < " << instr.arg2 << ";";
            break;
        case TACOpcode::LTE:
            oss << "    " << instr.result << " = " << instr.arg1 << " <= " << instr.arg2 << ";";
            break;
        case TACOpcode::GT:
            oss << "    " << instr.result << " = " << instr.arg1 << " > " << instr.arg2 << ";";
            break;
        case TACOpcode::GTE:
            oss << "    " << instr.result << " = " << instr.arg1 << " >= " << instr.arg2 << ";";
            break;
        case TACOpcode::ASSIGN:
            oss << "    " << instr.result << " = " << instr.arg1 << ";";
            break;
        case TACOpcode::LABEL:
            oss << instr.result << ":";
            break;
        case TACOpcode::GOTO:
            oss << "    goto " << instr.result << ";";
            break;
        case TACOpcode::IF_FALSE:
            oss << "    if (!" << instr.arg1 << ") goto " << instr.result << ";";
            break;
        case TACOpcode::IF_TRUE:
            oss << "    if (" << instr.arg1 << ") goto " << instr.result << ";";
            break;
        case TACOpcode::RETURN:
            if (!instr.arg1.empty())
                oss << "    return " << instr.arg1 << ";";
            else
                oss << "    return 0;";
            break;
        case TACOpcode::NOP:
            oss << "    // nop";
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
