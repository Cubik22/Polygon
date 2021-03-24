#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>


enum class LogLevel{
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3
};

class LOG{

public:
    LOG(LogLevel level);
    ~LOG();

    template<typename T>
    LOG& operator<<(const T& message){
        if (logLevel >= LEVEL){
            opened = true;
            if (logLevel < LogLevel::WARN){
                std::cout << message;
            } else{
                std::cerr << message;
            }
        }
        return *this;
    }

    static LogLevel LEVEL;

private:
    bool opened;
    LogLevel logLevel;

    inline std::string getLabel(LogLevel level);
};

// DEPRECATED

class LoggerStatic{

public:
    static bool LevelMessage;
    static bool LevelWarning;
    static bool LevelError;

    static void Message(const std::string& line);
    static void Message(const char* line);
    static void Warning(const std::string& line);
    static void Warning(const char* line);
    static void Error(const std::string& line);
    static void Error(const char* line);
};

#endif // LOGGER_H
