#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>
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

private:
    Logger() {}
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    MutexType logMutex;
};

#endif // LOGGER_H
