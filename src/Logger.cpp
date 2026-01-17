#include "Logger.h"
#include <ctime>
#include <iomanip>

void Logger::log(LogLevel level, const std::string& message) {
    LockType lock(logMutex);

    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);

    std::cout << "[" << std::put_time(localTime, "%Y-%m-%d %H:%M:%S") << "] ";

    switch (level) {
        case LogLevel::INFO: std::cout << "[INFO] "; break;
        case LogLevel::WARNING: std::cout << "[WARNING] "; break;
        case LogLevel::ERROR: std::cout << "[ERROR] "; break;
        case LogLevel::DEBUG: std::cout << "[DEBUG] "; break;
    }

    std::cout << message << std::endl;
}
