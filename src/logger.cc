#include "logger.h"
#include <time.h>
#include <iostream>
#include <fstream>

Logger::Logger()
{
    m_logLevel = INFO;
    std::thread writeLog([&](){
        for(;;)
        {
            time_t now = time(nullptr);
            tm* now_tm = localtime(&now);

            char file_name[128];
            sprintf(file_name, "%s%d-%d-%d-log.txt", m_logPath.c_str(),
                                                    now_tm->tm_year + 1900,
                                                    now_tm->tm_mon + 1,
                                                    now_tm->tm_mday);
            std::ofstream ofs(file_name, std::ios::app | std::ios::out);
            if(!ofs.is_open())
            {
                std::cout << "file_name: " << file_name << "open failed" << std::endl;
            }

            std::string msg = m_logQue.pop();
            char buf[64];
            sprintf(buf, "%d:%d:%d => ", now_tm->tm_hour,
                                        now_tm->tm_min,
                                        now_tm->tm_sec);
            msg.insert(0, buf);
            ofs << msg << '\n';
            ofs.close();
        }
    });
    writeLog.detach();
}

Logger& Logger::GetInstance()
{
    static Logger logger;
    return logger;
}
void Logger::SetLogLevel(LogLevel level)
{
    m_logLevel = level;
}

void Logger::SetLogPath(std::string path)
{
    m_logPath = path;
}

void Logger::Log(std::string msg, LogLevel logLevel)
{
    if(logLevel > this->m_logLevel) return;
    m_logQue.push(msg);
}