#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>

#define RESET   "\033[0m"
#define BLUE    "\033[34m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RED     "\033[31m"


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
        if (logLevel >= LOG::LEVEL){
            opened = true;
            if (logLevel == LogLevel::DEBUG){
                std::cout << BLUE << message << RESET;
            } else if (logLevel == LogLevel::INFO){
                std::cout << GREEN << message << RESET;
            } else if (logLevel == LogLevel::WARN){
                std::cerr << YELLOW << message << RESET;
            } else{
                std::cerr << RED << message << RESET;
            }
        }
        return *this;
    }

    static LogLevel LEVEL;

    static void NewLine(LogLevel level = LogLevel::ERROR);

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
