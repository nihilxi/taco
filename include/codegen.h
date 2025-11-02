#ifndef CODEGEN_H
#define CODEGEN_H

#include <string>
#include <vector>
#include <set>
#include "tac.h"

// C Code Generator from TAC
class CCodeGenerator
{
private:
    std::set<std::string> variables;
    std::set<std::string> temporaries;
    
    void collectVariables(const std::vector<TACInstruction>& tac);
    std::string generateVariableDeclarations();
    std::string generateInstruction(const TACInstruction& instr);
    
public:
    CCodeGenerator();
    
    std::string generate(const std::vector<TACInstruction>& tac);
    void writeToFile(const std::string& code, const std::string& filename);
};

#endif // CODEGEN_H
