#include <iostream>
#include <iomanip>
#include "../include/energy.h"

// Constructor
EnergyModel::EnergyModel()
{
    initializeEnergyTable();
}

// Initialize energy costs for different instructions
// Values are based on relative CPU instruction costs
// Reference: typical x86-64 instruction latencies and energy consumption
void EnergyModel::initializeEnergyTable()
{
    // Arithmetic operations (base cost in energy units)
    energyTable[TACOpcode::ADD] = {1.0, 0.5, "Integer addition"};
    energyTable[TACOpcode::SUB] = {1.0, 0.5, "Integer subtraction"};
    energyTable[TACOpcode::MUL] = {3.0, 0.5, "Integer multiplication (higher latency)"};
    energyTable[TACOpcode::DIV] = {20.0, 0.5, "Integer division (very expensive)"};
    energyTable[TACOpcode::MOD] = {20.0, 0.5, "Modulo operation (similar to division)"};
    energyTable[TACOpcode::NEG] = {1.0, 0.5, "Negation"};
    
    // Logical operations
    energyTable[TACOpcode::AND] = {1.0, 0.5, "Logical AND"};
    energyTable[TACOpcode::OR] = {1.0, 0.5, "Logical OR"};
    energyTable[TACOpcode::NOT] = {1.0, 0.5, "Logical NOT"};
    
    // Comparison operations
    energyTable[TACOpcode::EQ] = {1.0, 0.5, "Equality comparison"};
    energyTable[TACOpcode::NEQ] = {1.0, 0.5, "Inequality comparison"};
    energyTable[TACOpcode::LT] = {1.0, 0.5, "Less than comparison"};
    energyTable[TACOpcode::LTE] = {1.0, 0.5, "Less than or equal comparison"};
    energyTable[TACOpcode::GT] = {1.0, 0.5, "Greater than comparison"};
    energyTable[TACOpcode::GTE] = {1.0, 0.5, "Greater than or equal comparison"};
    
    // Assignment
    energyTable[TACOpcode::ASSIGN] = {0.5, 0.5, "Assignment (register move)"};
    
    // Control flow (includes pipeline flush costs)
    energyTable[TACOpcode::LABEL] = {0.0, 0.0, "Label (no cost)"};
    energyTable[TACOpcode::GOTO] = {5.0, 0.0, "Unconditional jump (pipeline flush)"};
    energyTable[TACOpcode::IF_FALSE] = {3.0, 0.5, "Conditional branch"};
    energyTable[TACOpcode::IF_TRUE] = {3.0, 0.5, "Conditional branch"};
    
    // Function operations
    energyTable[TACOpcode::PARAM] = {1.0, 1.0, "Parameter passing (stack/register)"};
    energyTable[TACOpcode::CALL] = {10.0, 2.0, "Function call (context switch)"};
    energyTable[TACOpcode::RETURN] = {5.0, 1.0, "Function return"};
    
    // Array operations (memory intensive)
    energyTable[TACOpcode::INDEX] = {2.0, 3.0, "Array indexing (memory access)"};
    energyTable[TACOpcode::STORE] = {2.0, 3.0, "Array store (memory write)"};
    
    // Special
    energyTable[TACOpcode::NOP] = {0.1, 0.0, "No operation"};
}

// Get energy cost for a single instruction
double EnergyModel::getInstructionCost(const TACInstruction& instr) const
{
    auto it = energyTable.find(instr.opcode);
    if (it != energyTable.end())
    {
        return it->second.baseCost + it->second.memoryAccess;
    }
    return 1.0; // Default cost
}

// Calculate total energy for a program
double EnergyModel::calculateProgramEnergy(const std::vector<TACInstruction>& tac) const
{
    double totalEnergy = 0.0;
    
    for (const auto& instr : tac)
    {
        totalEnergy += getInstructionCost(instr);
    }
    
    return totalEnergy;
}

// Print detailed energy report
void EnergyModel::printEnergyReport(const std::vector<TACInstruction>& tac) const
{
    std::cout << "\n=== ENERGY CONSUMPTION REPORT ===" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    
    // Count instructions by type
    std::map<TACOpcode, int> instructionCounts;
    std::map<TACOpcode, double> energyByType;
    
    for (const auto& instr : tac)
    {
        instructionCounts[instr.opcode]++;
        energyByType[instr.opcode] += getInstructionCost(instr);
    }
    
    // Print summary
    std::cout << "\nInstruction-wise breakdown:" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    std::cout << std::left << std::setw(25) << "Instruction Type"
              << std::setw(10) << "Count"
              << std::setw(15) << "Unit Cost"
              << std::setw(15) << "Total Cost"
              << std::setw(15) << "% of Total" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    
    double totalEnergy = calculateProgramEnergy(tac);
    
    for (const auto& entry : instructionCounts)
    {
        auto it = energyTable.find(entry.first);
        if (it != energyTable.end())
        {
            double unitCost = it->second.baseCost + it->second.memoryAccess;
            double typeCost = energyByType[entry.first];
            double percentage = (typeCost / totalEnergy) * 100.0;
            
            std::cout << std::left << std::setw(25) << it->second.description
                      << std::setw(10) << entry.second
                      << std::setw(15) << unitCost
                      << std::setw(15) << typeCost
                      << std::setw(15) << percentage << std::endl;
        }
    }
    
    std::cout << std::string(80, '-') << std::endl;
    std::cout << "Total instructions: " << tac.size() << std::endl;
    std::cout << "Total energy cost: " << totalEnergy << " units" << std::endl;
    std::cout << "Average cost per instruction: " << (totalEnergy / tac.size()) << " units" << std::endl;
}

// Print energy cost table
void EnergyModel::printEnergyTable() const
{
    std::cout << "\n=== ENERGY COST TABLE ===" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << std::left << std::setw(30) << "Operation"
              << std::setw(12) << "Base Cost"
              << std::setw(12) << "Mem Cost"
              << std::setw(12) << "Total" << std::endl;
    std::cout << std::string(66, '-') << std::endl;
    
    for (const auto& entry : energyTable)
    {
        std::cout << std::left << std::setw(30) << entry.second.description
                  << std::setw(12) << entry.second.baseCost
                  << std::setw(12) << entry.second.memoryAccess
                  << std::setw(12) << (entry.second.baseCost + entry.second.memoryAccess)
                  << std::endl;
    }
}

// Energy Profiler Implementation

EnergyProfiler::EnergyProfiler() {}

void EnergyProfiler::profileInstruction(const TACInstruction& instr, int executionCount)
{
    double cost = model.getInstructionCost(instr) * executionCount;
    
    if (profiles.find(instr.opcode) == profiles.end())
    {
        profiles[instr.opcode] = {instr.opcode, executionCount, cost};
    }
    else
    {
        profiles[instr.opcode].executionCount += executionCount;
        profiles[instr.opcode].totalEnergy += cost;
    }
}

void EnergyProfiler::reset()
{
    profiles.clear();
}

void EnergyProfiler::printProfile() const
{
    std::cout << "\n=== ENERGY PROFILE ===" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << std::left << std::setw(25) << "Operation"
              << std::setw(15) << "Executions"
              << std::setw(15) << "Total Energy" << std::endl;
    std::cout << std::string(55, '-') << std::endl;
    
    for (const auto& entry : profiles)
    {
        std::cout << std::left << std::setw(25) << static_cast<int>(entry.first)
                  << std::setw(15) << entry.second.executionCount
                  << std::setw(15) << entry.second.totalEnergy << std::endl;
    }
    
    std::cout << std::string(55, '-') << std::endl;
    std::cout << "Total energy: " << getTotalEnergy() << " units" << std::endl;
}

double EnergyProfiler::getTotalEnergy() const
{
    double total = 0.0;
    for (const auto& entry : profiles)
    {
        total += entry.second.totalEnergy;
    }
    return total;
}
