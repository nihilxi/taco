#include <iostream>
#include <iomanip>
#include "../include/energy.h"
#include "../include/logger.h"

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
    
    // Assignment
    energyTable[TACOpcode::ASSIGN] = {0.5, 0.5, "Assignment (register move)"};
    
    // I/O
    energyTable[TACOpcode::PRINT] = {10.0, 2.0, "Print output (I/O operation)"};
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
    logger << "\n=== ENERGY CONSUMPTION REPORT ===" << std::endl;
    logger << std::fixed << std::setprecision(2);
    
    // Count instructions by type
    std::map<TACOpcode, int> instructionCounts;
    std::map<TACOpcode, double> energyByType;
    
    for (const auto& instr : tac)
    {
        instructionCounts[instr.opcode]++;
        energyByType[instr.opcode] += getInstructionCost(instr);
    }
    
    // Print summary
    logger << "\nInstruction-wise breakdown:" << std::endl;
    logger << std::string(80, '-') << std::endl;
    logger << std::left << std::setw(25) << "Instruction Type"
              << std::setw(10) << "Count"
              << std::setw(15) << "Unit Cost"
              << std::setw(15) << "Total Cost"
              << std::setw(15) << "% of Total" << std::endl;
    logger << std::string(80, '-') << std::endl;
    
    double totalEnergy = calculateProgramEnergy(tac);
    
    for (const auto& entry : instructionCounts)
    {
        auto it = energyTable.find(entry.first);
        if (it != energyTable.end())
        {
            double unitCost = it->second.baseCost + it->second.memoryAccess;
            double typeCost = energyByType[entry.first];
            double percentage = (typeCost / totalEnergy) * 100.0;
            
            logger << std::left << std::setw(25) << it->second.description
                      << std::setw(10) << entry.second
                      << std::setw(15) << unitCost
                      << std::setw(15) << typeCost
                      << std::setw(15) << percentage << std::endl;
        }
    }
    
    logger << std::string(80, '-') << std::endl;
    logger << "Total instructions: " << tac.size() << std::endl;
    logger << "Total energy cost: " << totalEnergy << " units" << std::endl;
    logger << "Average cost per instruction: " << (totalEnergy / tac.size()) << " units" << std::endl;
}

// Print energy cost table
void EnergyModel::printEnergyTable() const
{
    logger << "\n=== ENERGY COST TABLE ===" << std::endl;
    logger << std::fixed << std::setprecision(2);
    logger << std::left << std::setw(30) << "Operation"
              << std::setw(12) << "Base Cost"
              << std::setw(12) << "Mem Cost"
              << std::setw(12) << "Total" << std::endl;
    logger << std::string(66, '-') << std::endl;
    
    for (const auto& entry : energyTable)
    {
        logger << std::left << std::setw(30) << entry.second.description
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
    logger << "\n=== ENERGY PROFILE ===" << std::endl;
    logger << std::fixed << std::setprecision(2);
    logger << std::left << std::setw(25) << "Operation"
              << std::setw(15) << "Executions"
              << std::setw(15) << "Total Energy" << std::endl;
    logger << std::string(55, '-') << std::endl;
    
    for (const auto& entry : profiles)
    {
        logger << std::left << std::setw(25) << static_cast<int>(entry.first)
                  << std::setw(15) << entry.second.executionCount
                  << std::setw(15) << entry.second.totalEnergy << std::endl;
    }
    
    logger << std::string(55, '-') << std::endl;
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
