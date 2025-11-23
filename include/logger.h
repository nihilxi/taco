#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <vector>

// Structure to store timing information for each stage
struct TimingInfo
{
    std::string stageName;
    double durationMs;
};

// Structure to store resource usage information
struct ResourceInfo
{
    std::string stageName;
    double durationMs;
    double cpuUsagePercent;
    long memoryUsageKB;
    double powerUsageWatts;
};

// Structure to store process stats
struct ProcessStats
{
    long memoryKB;
    double cpuPercent;
    unsigned long long cpuTimeJiffies;
};

// Logger class for redirecting all output to a file
class Logger
{
private:
    std::ofstream logFile;
    bool isOpen;
    std::string filename;
    std::vector<TimingInfo> timings;
    std::vector<ResourceInfo> resourceTimings;
    std::chrono::high_resolution_clock::time_point stageStartTime;
    bool profilingEnabled;
    ProcessStats startStats;
    std::chrono::high_resolution_clock::time_point profileStartTime;
    
    // Helper methods for resource profiling
    ProcessStats getProcessStats();
    double getPowerUsage();
    
public:
    Logger();
    ~Logger();
    
    bool open(const std::string& filename);
    void close();
    
    // Template method for flexible logging
    template<typename T>
    Logger& operator<<(const T& msg)
    {
        if (isOpen)
        {
            logFile << msg;
            logFile.flush(); // Ensure immediate write
        }
        return *this;
    }
    
    // Handle stream manipulators (std::endl, std::fixed, etc.)
    Logger& operator<<(std::ostream& (*manip)(std::ostream&))
    {
        if (isOpen)
        {
            manip(logFile);
            logFile.flush();
        }
        return *this;
    }
    
    // Handle iOS base manipulators (std::setw, std::setprecision, etc.)
    Logger& operator<<(std::ios_base& (*manip)(std::ios_base&))
    {
        if (isOpen)
        {
            manip(logFile);
            logFile.flush();
        }
        return *this;
    }
    
    bool isFileOpen() const { return isOpen; }
    std::string getFilename() const { return filename; }
    
    // Timing methods
    void startTimer();
    void endTimer(const std::string& stageName);
    void printTimingReport(); // Prints to both console and log
    void printTimingReportToConsole(); // Prints only to console
    void printTimingReportToLog(); // Prints only to log file
    double getTotalTime() const;
    void clearTimings();
    
    // Resource profiling methods
    void enableProfiling(bool enable = true);
    void startProfiling();
    void endProfiling(const std::string& stageName);
    void printProfilingReport();
    void printProfilingReportToConsole(); // Prints only to console
    void printProfilingReportToLog(); // Prints only to log file
};

// Global logger instance
extern Logger logger;

#endif // LOGGER_H
