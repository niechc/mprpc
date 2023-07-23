#pragma once

#include <string>
#include "logqueue.h"

enum LogLevel
{
    // infomation
    INFO = 0, 
    // error
    ERROR = 1 
};

class Logger
{
public:
    // get logger instance
    static Logger& GetInstance();

    // set log level INFO or ERROR
    void SetLogLevel(LogLevel level);

    // set log path
    void SetLogPath(std::string path);

    // write log
    void Log(std::string msg, LogLevel logLevel);
private:
    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;

    std::string m_logPath;
    LogLevel m_logLevel;
    LogQueue<std::string> m_logQue;
};

// write information of level INFO
#define LOG_INFO(logmsgformat, ...)                          \
    do                                                       \
    {                                                        \
        Logger& logger = Logger::GetInstance();              \
        char c[1024]{0};                                     \
        sprintf(c, "[INFO]:");                               \
        sprintf(c + strlen(c), logmsgformat, ##__VA_ARGS__); \
        logger.Log(c, INFO);                                 \
    } while (0)

// write information of level ERROR
#define LOG_ERROR(logmsgformat, ...)                         \
    do                                                       \
    {                                                        \
        Logger& logger = Logger::GetInstance();              \
        char c[1024]{0};                                     \
        sprintf(c, "[ERROR]:");                              \
        sprintf(c + strlen(c), logmsgformat, ##__VA_ARGS__); \
        logger.Log(c, ERROR);                                \
    } while (0)

