#include "../include/logger.h"

// Global logger instance
Logger logger;

Logger::Logger() : isOpen(false), filename(), timings() {}

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
    
    // Log to file and console
    std::cout << "[TIMING] " << stageName << ": " << std::fixed << std::setprecision(3) 
              << durationMs << " ms" << std::endl;
    
    if (isOpen)
    {
        logFile << "[TIMING] " << stageName << ": " << std::fixed << std::setprecision(3) 
                << durationMs << " ms" << std::endl;
        logFile.flush();
    }
}

void Logger::printTimingReport()
{
    std::cout << "\n=== TIMING REPORT ===" << std::endl;
    if (isOpen)
    {
        logFile << "\n=== TIMING REPORT ===" << std::endl;
    }
    
    double totalTime = 0.0;
    for (const auto& timing : timings)
    {
        std::cout << std::left << std::setw(30) << timing.stageName << ": " 
                  << std::fixed << std::setprecision(3) << std::right << std::setw(10) 
                  << timing.durationMs << " ms" << std::endl;
        
        if (isOpen)
        {
            logFile << std::left << std::setw(30) << timing.stageName << ": " 
                    << std::fixed << std::setprecision(3) << std::right << std::setw(10) 
                    << timing.durationMs << " ms" << std::endl;
        }
        
        totalTime += timing.durationMs;
    }
    
    std::cout << std::string(44, '-') << std::endl;
    std::cout << std::left << std::setw(30) << "Total" << ": " 
              << std::fixed << std::setprecision(3) << std::right << std::setw(10) 
              << totalTime << " ms" << std::endl;
    std::cout << std::string(44, '=') << std::endl;
    
    if (isOpen)
    {
        logFile << std::string(44, '-') << std::endl;
        logFile << std::left << std::setw(30) << "Total" << ": " 
                << std::fixed << std::setprecision(3) << std::right << std::setw(10) 
                << totalTime << " ms" << std::endl;
        logFile << std::string(44, '=') << std::endl;
        logFile.flush();
    }
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
