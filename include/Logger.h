#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>
#include <atomic>
#include "ThreadCompat.h"

#include <chrono>

enum class LogLevel {
    INFO,
    WARNING,
    ERROR,
    DEBUG
};

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void log(LogLevel level, const std::string& message);
    
    void setQuiet(bool quiet) {
        this->quiet = quiet;
    }

private:
    Logger() : quiet(false) {}
    std::atomic<bool> quiet; // Use atomic for thread safety without mutex complexity here
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    MutexType logMutex;
};

#endif // LOGGER_H
