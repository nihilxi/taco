#include "../include/logger.h"
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <cmath>

// Global logger instance
Logger logger;

Logger::Logger() : isOpen(false), filename(), timings(), resourceTimings(), profilingEnabled(false) {}

Logger::~Logger()
{
    close();
}

bool Logger::open(const std::string& fname)
{
    close(); // Close if already open
    
    filename = fname;
    logFile.open(filename);
    isOpen = logFile.is_open();
    
    if (isOpen)
    {
        // Write header
        logFile << "==============================================\n";
        logFile << "  TACO Compiler - Compilation Log\n";
        logFile << "==============================================\n\n";
        logFile.flush();
    }
    
    return isOpen;
}

void Logger::close()
{
    if (isOpen)
    {
        logFile << "\n==============================================\n";
        logFile << "  End of compilation log\n";
        logFile << "==============================================\n";
        logFile.close();
        isOpen = false;
    }
}

void Logger::startTimer()
{
    stageStartTime = std::chrono::high_resolution_clock::now();
}

void Logger::endTimer(const std::string& stageName)
{
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - stageStartTime);
    double durationMs = duration.count() / 1000.0;
    
    timings.push_back({stageName, durationMs});
}

void Logger::printTimingReport()
{
    printTimingReportToConsole();
    if (isOpen)
    {
        printTimingReportToLog();
    }
}

void Logger::printTimingReportToConsole()
{
    std::cout << "\n=== TIMING REPORT ===" << std::endl;
    
    double totalTime = 0.0;
    for (const auto& timing : timings)
    {
        std::cout << std::left << std::setw(30) << timing.stageName << ": " 
                  << std::fixed << std::setprecision(3) << std::right << std::setw(10) 
                  << timing.durationMs << " ms" << std::endl;
        
        totalTime += timing.durationMs;
    }
    
    std::cout << std::string(44, '-') << std::endl;
    std::cout << std::left << std::setw(30) << "Total" << ": " 
              << std::fixed << std::setprecision(3) << std::right << std::setw(10) 
              << totalTime << " ms" << std::endl;
    std::cout << std::string(44, '=') << std::endl;
}

void Logger::printTimingReportToLog()
{
    if (!isOpen)
        return;
        
    logFile << "\n=== TIMING REPORT ===" << std::endl;
    
    double totalTime = 0.0;
    for (const auto& timing : timings)
    {
        logFile << std::left << std::setw(30) << timing.stageName << ": " 
                << std::fixed << std::setprecision(3) << std::right << std::setw(10) 
                << timing.durationMs << " ms" << std::endl;
        
        totalTime += timing.durationMs;
    }
    
    logFile << std::string(44, '-') << std::endl;
    logFile << std::left << std::setw(30) << "Total" << ": " 
            << std::fixed << std::setprecision(3) << std::right << std::setw(10) 
            << totalTime << " ms" << std::endl;
    logFile << std::string(44, '=') << std::endl;
    logFile.flush();
}

double Logger::getTotalTime() const
{
    double total = 0.0;
    for (const auto& timing : timings)
    {
        total += timing.durationMs;
    }
    return total;
}

void Logger::clearTimings()
{
    timings.clear();
    resourceTimings.clear();
}

void Logger::enableProfiling(bool enable)
{
    profilingEnabled = enable;
}

ProcessStats Logger::getProcessStats()
{
    ProcessStats stats = {0, 0.0, 0};
    
    // Read /proc/self/stat for CPU and memory usage
    std::ifstream statFile("/proc/self/stat");
    if (!statFile.is_open())
        return stats;
    
    std::string line;
    std::getline(statFile, line);
    statFile.close();
    
    std::istringstream iss(line);
    std::string dummy;
    unsigned long long utime, stime;
    long rss;
    
    // Parse /proc/self/stat (fields: pid, comm, state, ... utime(14), stime(15), ... rss(24))
    for (int i = 0; i < 13; i++) iss >> dummy;
    iss >> utime >> stime;
    for (int i = 0; i < 8; i++) iss >> dummy;
    iss >> rss;
    
    stats.cpuTimeJiffies = utime + stime;
    stats.memoryKB = rss * sysconf(_SC_PAGESIZE) / 1024;
    
    return stats;
}

double Logger::getPowerUsage()
{
    // Try to read power consumption from RAPL (Intel) or battery stats
    double watts = 0.0;
    
    // Try Intel RAPL interface first
    std::ifstream energyFile("/sys/class/powercap/intel-rapl:0/energy_uj");
    if (energyFile.is_open())
    {
        unsigned long long energy_uj;
        energyFile >> energy_uj;
        energyFile.close();
        
        // Convert microjoules to approximate watts (this is a snapshot, not rate)
        // For better accuracy, we'd need to measure over time
        watts = energy_uj / 1000000.0; // Simplified; actual implementation needs delta measurement
    }
    else
    {
        // Fallback: estimate based on CPU usage (very rough approximation)
        // Typical laptop CPU: 15-45W TDP, assume 30W average at 100% usage
        ProcessStats current = getProcessStats();
        watts = current.cpuPercent * 0.30; // Very rough estimate
    }
    
    return watts;
}

void Logger::startProfiling()
{
    if (!profilingEnabled)
        return;
        
    profileStartTime = std::chrono::high_resolution_clock::now();
    startStats = getProcessStats();
}

void Logger::endProfiling(const std::string& stageName)
{
    if (!profilingEnabled)
        return;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - profileStartTime);
    double durationMs = duration.count() / 1000.0;
    
    ProcessStats endStats = getProcessStats();
    
    // Calculate CPU usage percentage
    long clockTicksPerSec = sysconf(_SC_CLK_TCK);
    unsigned long long cpuDelta = endStats.cpuTimeJiffies - startStats.cpuTimeJiffies;
    double timeSec = durationMs / 1000.0;
    
    // For very short durations, use an alternative approach
    double cpuPercent;
    if (timeSec < 0.001 || cpuDelta == 0) {
        // Use a simplified estimation based on the assumption that compilation is CPU-bound
        cpuPercent = (durationMs > 0.1) ? 50.0 : 10.0; // Rough estimate
    } else {
        cpuPercent = (cpuDelta / (double)clockTicksPerSec) / timeSec * 100.0;
    }
    
    // Clamp CPU percentage to reasonable values
    if (cpuPercent > 100.0) cpuPercent = 100.0;
    if (cpuPercent < 0.0) cpuPercent = 0.0;
    
    // Get power usage (approximate)
    double powerWatts = cpuPercent * 0.30; // Rough estimate: 30W at 100% CPU
    
    ResourceInfo info;
    info.stageName = stageName;
    info.durationMs = durationMs;
    info.cpuUsagePercent = cpuPercent;
    info.memoryUsageKB = endStats.memoryKB;
    info.powerUsageWatts = powerWatts;
    
    resourceTimings.push_back(info);
}

void Logger::printProfilingReport()
{
    printProfilingReportToConsole();
    if (isOpen)
    {
        printProfilingReportToLog();
    }
}

void Logger::printProfilingReportToConsole()
{
    if (!profilingEnabled || resourceTimings.empty())
        return;
    
    std::cout << "\n=== RESOURCE PROFILING REPORT ===" << std::endl;
    
    // Header
    std::cout << std::left << std::setw(30) << "Stage"
              << std::right << std::setw(12) << "Time (ms)"
              << std::setw(12) << "CPU (%)"
              << std::setw(14) << "Memory (KB)"
              << std::setw(14) << "Power (W)" << std::endl;
    std::cout << std::string(82, '-') << std::endl;
    
    double totalTime = 0.0;
    double totalCpu = 0.0;
    long maxMemory = 0;
    double totalPower = 0.0;
    
    for (const auto& info : resourceTimings)
    {
        std::cout << std::left << std::setw(30) << info.stageName
                  << std::right << std::fixed << std::setprecision(2)
                  << std::setw(12) << info.durationMs
                  << std::setw(12) << info.cpuUsagePercent
                  << std::setw(14) << info.memoryUsageKB
                  << std::setw(14) << info.powerUsageWatts << std::endl;
        
        totalTime += info.durationMs;
        totalCpu += info.cpuUsagePercent;
        if (info.memoryUsageKB > maxMemory)
            maxMemory = info.memoryUsageKB;
        totalPower += info.powerUsageWatts;
    }
    
    std::cout << std::string(82, '-') << std::endl;
    std::cout << std::left << std::setw(30) << "Totals/Averages"
              << std::right << std::fixed << std::setprecision(2)
              << std::setw(12) << totalTime
              << std::setw(12) << (totalCpu / resourceTimings.size())
              << std::setw(14) << maxMemory
              << std::setw(14) << (totalPower / resourceTimings.size()) << std::endl;
    std::cout << std::string(82, '=') << std::endl;
}

void Logger::printProfilingReportToLog()
{
    if (!isOpen || !profilingEnabled || resourceTimings.empty())
        return;
    
    logFile << "\n=== RESOURCE PROFILING REPORT ===" << std::endl;
    
    // Header
    logFile << std::left << std::setw(30) << "Stage"
            << std::right << std::setw(12) << "Time (ms)"
            << std::setw(12) << "CPU (%)"
            << std::setw(14) << "Memory (KB)"
            << std::setw(14) << "Power (W)" << std::endl;
    logFile << std::string(82, '-') << std::endl;
    
    double totalTime = 0.0;
    double totalCpu = 0.0;
    long maxMemory = 0;
    double totalPower = 0.0;
    
    for (const auto& info : resourceTimings)
    {
        logFile << std::left << std::setw(30) << info.stageName
                << std::right << std::fixed << std::setprecision(2)
                << std::setw(12) << info.durationMs
                << std::setw(12) << info.cpuUsagePercent
                << std::setw(14) << info.memoryUsageKB
                << std::setw(14) << info.powerUsageWatts << std::endl;
        
        totalTime += info.durationMs;
        totalCpu += info.cpuUsagePercent;
        if (info.memoryUsageKB > maxMemory)
            maxMemory = info.memoryUsageKB;
        totalPower += info.powerUsageWatts;
    }
    
    logFile << std::string(82, '-') << std::endl;
    logFile << std::left << std::setw(30) << "Totals/Averages"
            << std::right << std::fixed << std::setprecision(2)
            << std::setw(12) << totalTime
            << std::setw(12) << (totalCpu / resourceTimings.size())
            << std::setw(14) << maxMemory
            << std::setw(14) << (totalPower / resourceTimings.size()) << std::endl;
    logFile << std::string(82, '=') << std::endl;
    logFile.flush();
}
