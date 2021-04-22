#include "Logger.h"

LOG::LOG(LogLevel level) : opened{false}{
    logLevel = level;

    if (level == LogLevel::WARN || level == LogLevel::ERROR ||
       (level == LogLevel::INFO && LOG::PRINT_INFO_LEVEL) || (level == LogLevel::DEBUG && LOG::PRINT_DEBUG_LEVEL)){
        operator<<("["+getLabel(level)+"] ");
    }
}

LOG::~LOG(){
    if (opened){
        if (logLevel < LogLevel::WARN){
            std::cout << "\n";
        } else{
            std::cerr << "\n";
        }
    }
    opened = false;
}

void LOG::NewLine(LogLevel level){
    if (level >= LOG::LEVEL){
        std::cout << std::endl;
    }
}

LogLevel LOG::LEVEL = LogLevel::DEBUG;

bool LOG::PRINT_DEBUG_LEVEL = true;
bool LOG::PRINT_INFO_LEVEL = true;

std::string LOG::getLabel(LogLevel level){
    if (level == LogLevel::DEBUG){
        return "DEBUG";
    } else if (level == LogLevel::INFO){
        return "INFO";
    } else if (level == LogLevel::WARN){
        return "WARN";
    } else{
        return "ERROR";
    }
}

// DEPRECATED

bool LoggerStatic::LevelMessage = true;
bool LoggerStatic::LevelWarning = true;
bool LoggerStatic::LevelError = true;

void LoggerStatic::Message(const std::string& line){
    if (LevelMessage){
        std::cout << line << "\n";
    }
}

void LoggerStatic::Message(const char* line){
    if (LevelMessage){
        std::cout << line << "\n";
    }
}

void LoggerStatic::Warning(const std::string& line){
    if (LevelWarning){
        std::cerr << "WARNING: " << line << "\n";
    }
}

void LoggerStatic::Warning(const char* line){
    if (LevelWarning){
        std::cerr << "WARNING: " << line << "\n";
    }
}

void LoggerStatic::Error(const std::string& line){
    if (LevelError){
        std::cerr << "ERROR: " << line << "\n";
    }
}

void LoggerStatic::Error(const char* line){
    if (LevelError){
        std::cerr << "ERROR: " << line << "\n";
    }
}

