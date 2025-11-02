#ifndef ENERGY_H
#define ENERGY_H

#include <string>
#include <map>
#include <vector>
#include "tac.h"

// Energy cost model (in arbitrary energy units)
// Based on typical CPU instruction costs
struct EnergyCost
{
    double baseCost;        // Base energy cost
    double memoryAccess;    // Additional cost for memory access
    std::string description;
};

class EnergyModel
{
private:
    std::map<TACOpcode, EnergyCost> energyTable;
    
    void initializeEnergyTable();
    
public:
    EnergyModel();
    
    double getInstructionCost(const TACInstruction& instr) const;
    double calculateProgramEnergy(const std::vector<TACInstruction>& tac) const;
    
    void printEnergyReport(const std::vector<TACInstruction>& tac) const;
    void printEnergyTable() const;
};

// Energy profiler - tracks energy consumption during execution
class EnergyProfiler
{
private:
    struct InstructionProfile
    {
        TACOpcode opcode;
        int executionCount;
        double totalEnergy;
    };
    
    std::map<TACOpcode, InstructionProfile> profiles;
    EnergyModel model;
    
public:
    EnergyProfiler();
    
    void profileInstruction(const TACInstruction& instr, int executionCount = 1);
    void reset();
    
    void printProfile() const;
    double getTotalEnergy() const;
};

#endif // ENERGY_H
