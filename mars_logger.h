#ifndef MARS_LOGGER_H_
#define MARS_LOGGER_H_

#include <iostream>
#include <string>
#include <json/json.h>
#include <fstream>
#include <mutex>
#include <filesystem>
#include <fmt/core.h>

#define LOG_CONFIG_PATH "./logconf.json"

using std::string;

namespace mars {

enum class LogLevel : int { FATAL = 0, ERROR, WARN, INFO, DEBUG, TRACE };

//                                    0          1     2      3     4      5
enum class TerminalLogLevel : int { FATAL = 0, ERROR, WARN, INFO, DEBUG, TRACE };

//                                0          1     2      3     4      5
enum class FileLogLevel : int { FATAL = 0, ERROR, WARN, INFO, DEBUG, TRACE };

struct LoggerConfig {
    bool    logSwitch;
    bool    logTerminalSwitch;
    string  logTerminalLevel;
    bool    logFileSwitch; 
    string  logFileLevel;
    string  logFileName;
    string  logFilePath;
    string  logFileMaxSize;
    string  logFileReachMaxBehavior;
};

#ifndef LOG_LEVEL
#define LOG_LEVEL INFO //默认日志级别
#endif

class MarsLogger {
public:
    void initLogConfig ();

    void releaseLogConfig ();

    std::string LogHead(LogLevel lvl, const char *file_name, const char *func_name, int line_no);

    bool ifFileOutPut (FileLogLevel fileLogLevel);

    bool ifTerminalOutPut (TerminalLogLevel terminalLogLevel);

    std::string getLogFileNameTime ();

    std::string getLogOutPutTime ();

    std::string getLogFileName() {
        return loggerConfig.logFileName;
    }
    
    std::ofstream & getFile () {
        return file;
    }

    bool LoggerStart() {
        return loggerConfig.logSwitch;
    }

    static MarsLogger * getInstance ();

    void bindFileOutPutLevelMap (std::string json_value, FileLogLevel fileLogLevel);

    void bindTerminalOutPutLevelMap (std::string json_value, TerminalLogLevel terminalLogLevel);

    bool createFile (std::string path);

//用模板得到级别的字符串
template <typename T>
std::string getLogLevelStr (T level) {
    switch (int(level)) {
        case 0:    return "FATAL";
        case 1:    return "ERROR";
        case 2:    return "WARN";
        case 3:    return "INFO";
        case 4:    return "DEBUG";
        case 5:    return "TRACE";
        default:   return "UNKNOWN";
    }
}

private:
    LoggerConfig loggerConfig;
    static MarsLogger * single_instance;
    static std::mutex * mutex_new;
    std::map<FileLogLevel, bool> fileCoutMap;
    std::map<TerminalLogLevel, bool> terminalCoutMap;
    std::ostringstream stream;
    std::ofstream file;

private:
    MarsLogger();
    ~MarsLogger();
};


template <typename ...Args>
void Log_Trace(const char* fmt, const char* file_name, const char* func_name, int line_no, Args... args) {
    mars::MarsLogger* logger = mars::MarsLogger::getInstance();

    bool Terminal, File;

    Terminal = logger->ifTerminalOutPut(mars::TerminalLogLevel::TRACE);
    File = logger->ifFileOutPut(mars::FileLogLevel::TRACE);

    if (!Terminal || !File) {
        return;
    }

    std::string log = logger->LogHead(mars::LogLevel::INFO, file_name, func_name, line_no);

    log = log + fmt::format(fmt, args...);

    if (Terminal) {
        std::cout << log << '\n'; 
    }

    if (File) {
        logger->getFile() << log << '\n';
    }

    return;
}

template <typename ...Args>
void Log_Debug(const char* fmt, const char* file_name, const char* func_name, int line_no, Args... args) {
    mars::MarsLogger* logger = mars::MarsLogger::getInstance();


    bool Terminal, File;

    Terminal = logger->ifTerminalOutPut(mars::TerminalLogLevel::DEBUG);
    File = logger->ifFileOutPut(mars::FileLogLevel::DEBUG);

    if (!Terminal || !File) {
        return;
    }

    std::string log = logger->LogHead(mars::LogLevel::INFO, file_name, func_name, line_no);

    log = log + fmt::format(fmt, args...);

    if (Terminal) {
        std::cout << log << '\n'; 
    }

    if (File) {
        logger->getFile() << log << '\n';
    }

    return;
}

template <typename ...Args>
void Log_Info(const char* fmt, const char* file_name, const char* func_name, int line_no, Args... args) {
    mars::MarsLogger* logger = mars::MarsLogger::getInstance();

    if (!logger->LoggerStart())
    {
        return;
    }

    bool Terminal, File;

    Terminal = logger->ifTerminalOutPut(mars::TerminalLogLevel::INFO);
    File = logger->ifFileOutPut(mars::FileLogLevel::INFO);

    if (!Terminal && !File) {
        return;
    }

    //char buf[1024];
    //snprintf(buf, sizeof(buf), fmt, args...);

    //std::string log = logger->LogHead(mars::LogLevel::INFO, __FILE__, __func__, __LINE__) + buf;
    std::string log = logger->LogHead(mars::LogLevel::INFO, file_name, func_name, line_no);

    log = log + fmt::format(fmt, args...);

    if (Terminal) {
        std::cout << log << '\n'; 
    }

    if (File) {
        logger->getFile() << log << '\n';
    }

    return;
}

template <typename ...Args>
void Log_Warn(const char* fmt, const char* file_name, const char* func_name, int line_no, Args... args) {
    mars::MarsLogger* logger = mars::MarsLogger::getInstance();

    bool Terminal, File;

    Terminal = logger->ifTerminalOutPut(mars::TerminalLogLevel::WARN);
    File = logger->ifFileOutPut(mars::FileLogLevel::WARN);

    if (!Terminal || !File) {
        return;
    }

    std::string log = logger->LogHead(mars::LogLevel::INFO, file_name, func_name, line_no);

    log = log + fmt::format(fmt, args...);

    if (Terminal) {
        std::cout << log << '\n'; 
    }

    if (File) {
        logger->getFile() << log << '\n';
    }

    return;
}

template <typename ...Args>
void Log_Error(const char* fmt, const char* file_name, const char* func_name, int line_no, Args... args) {
    mars::MarsLogger* logger = mars::MarsLogger::getInstance();

    bool Terminal, File;

    Terminal = logger->ifTerminalOutPut(mars::TerminalLogLevel::ERROR);
    File = logger->ifFileOutPut(mars::FileLogLevel::ERROR);

    if (!Terminal || !File) {
        return;
    }

    std::string log = logger->LogHead(mars::LogLevel::INFO, file_name, func_name, line_no);

    log = log + fmt::format(fmt, args...);

    if (Terminal) {
        std::cout << log << '\n'; 
    }

    if (File) {
        logger->getFile() << log << '\n';
    }

    return;
}

template <typename ...Args>
void Log_Fatal(const char* fmt, const char* file_name, const char* func_name, int line_no, Args... args) {
    mars::MarsLogger* logger = mars::MarsLogger::getInstance();

    bool Terminal, File;

    Terminal = logger->ifTerminalOutPut(mars::TerminalLogLevel::FATAL);
    File = logger->ifFileOutPut(mars::FileLogLevel::FATAL);

    if (!Terminal || !File) {
        return;
    }

    std::string log = logger->LogHead(mars::LogLevel::INFO, file_name, func_name, line_no);

    log = log + fmt::format(fmt, args...);

    if (Terminal) {
        std::cout << log << '\n'; 
    }

    if (File) {
        logger->getFile() << log << '\n';
    }

    return;
}

#define LogInfo(fmt, ...) Log_Info(fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define LogWarn(fmt, ...) Log_Warn(fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define LogError(fmt, ...) Log_Error(fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define LogFatal(fmt, ...) Log_Fatal(fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define LogDebug(fmt, ...) Log_Debug(fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define LogTrace(fmt, ...) Log_Trace(fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__)

}
#endif
