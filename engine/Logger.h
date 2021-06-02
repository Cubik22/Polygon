#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>

#ifdef _WIN32

// renaming Polygon class in windows.h so it does not conflict with our Polygon
#define Polygon POLYGON
#include <windows.h>
#undef Polygon
// windows.h has also ERROR defined which conflits with LogLevel Error
#undef ERROR

#define RESET   15
#define BLUE    11
#define GREEN   10
#define YELLOW  14
#define RED     12

#else

#define RESET   "\033[0m"
#define BLUE    "\033[34m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RED     "\033[31m"

#endif

#define LDEB LOG(LogLevel::DEBUG)
#define LINF LOG(LogLevel::INFO)

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
#ifdef _WIN32
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            if (logLevel == LogLevel::DEBUG){
                SetConsoleTextAttribute(hConsole, BLUE);
                std::cout << message;
                SetConsoleTextAttribute(hConsole, RESET);
            } else if (logLevel == LogLevel::INFO){
                SetConsoleTextAttribute(hConsole, GREEN);
                std::cout << message;
                SetConsoleTextAttribute(hConsole, RESET);
            } else if (logLevel == LogLevel::WARN){
                SetConsoleTextAttribute(hConsole, YELLOW);
                std::cerr << message;
                SetConsoleTextAttribute(hConsole, RESET);
            } else{
                SetConsoleTextAttribute(hConsole, RED);
                std::cerr << message;
                SetConsoleTextAttribute(hConsole, RESET);
            }
#else
            if (logLevel == LogLevel::DEBUG){
                std::cout << BLUE << message << RESET;
            } else if (logLevel == LogLevel::INFO){
                std::cout << GREEN << message << RESET;
            } else if (logLevel == LogLevel::WARN){
                std::cerr << YELLOW << message << RESET;
            } else{
                std::cerr << RED << message << RESET;
            }
#endif
        }
        return *this;
    }

    static LogLevel LEVEL;

    // if you don't want to see [DEBUG] in front of every message
    static bool PRINT_DEBUG_LEVEL;
    // if you don't want to see [INFO] in front of every message
    static bool PRINT_INFO_LEVEL;

    // for WARN and ERROR, [WARN] and [ERROR] is always printed

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
