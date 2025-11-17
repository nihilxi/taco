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

// Logger class for redirecting all output to a file
class Logger
{
private:
    std::ofstream logFile;
    bool isOpen;
    std::string filename;
    std::vector<TimingInfo> timings;
    std::chrono::high_resolution_clock::time_point stageStartTime;
    
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
    void printTimingReport();
    double getTotalTime() const;
};

// Global logger instance
extern Logger logger;

#endif // LOGGER_H
