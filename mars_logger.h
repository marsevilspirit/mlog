#ifndef MARS_LOGGER_H_
#define MARS_LOGGER_H_

#include <iostream>
#include <string>
#include <json/json.h>
#include <fstream>
#include <mutex>
#include <filesystem>

#define LOG_CONFIG_PATH "./logconf.json"

using std::string;

namespace mars {

enum class LogLevel : int { FATAL = 0, ERROR, WARN, INFO, DEBUG, TRACE };

//                                    0          1     2      3     4      5
enum class TerminalLogLevel : int { FATAL = 0, ERROR, WARN, INFO, DEBUG, TRACE };

//                                0          1     2      3     4      5
enum class FileLogLevel : int { FATAL = 0, ERROR, WARN, INFO, DEBUG, TRACE };

struct LoggerConfig {
    string  logSwitch;
    string  logTerminalSwitch;
    string  logTerminalLevel;
    string  logFileSwitch; 
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

}

template <typename ...Args>
void LogTrace(const char* fmt, Args... args) {
    mars::MarsLogger* logger = mars::MarsLogger::getInstance();

    bool Terminal, File;

    Terminal = logger->ifTerminalOutPut(mars::TerminalLogLevel::TRACE);
    File = logger->ifFileOutPut(mars::FileLogLevel::TRACE);

    if (!Terminal || !File) {
        return;
    }

    char buf[1024];
    snprintf(buf, sizeof(buf), fmt, args...);

    std::string log = logger->LogHead(mars::LogLevel::TRACE, __FILE__, __func__, __LINE__) + buf;

    if (Terminal) {
        std::cout << log << std::endl; 
    }

    if (File) {
        logger->getFile() << log << std::endl;
    }

    return;
}

template <typename ...Args>
void LogDebug(const char* fmt, Args... args) {
    mars::MarsLogger* logger = mars::MarsLogger::getInstance();

    bool Terminal, File;

    Terminal = logger->ifTerminalOutPut(mars::TerminalLogLevel::DEBUG);
    File = logger->ifFileOutPut(mars::FileLogLevel::DEBUG);

    if (!Terminal || !File) {
        return;
    }

    char buf[1024];
    snprintf(buf, sizeof(buf), fmt, args...);

    std::string log = logger->LogHead(mars::LogLevel::DEBUG, __FILE__, __func__, __LINE__) + buf;

    if (Terminal) {
        std::cout << log << std::endl; 
    }

    if (File) {
        logger->getFile() << log << std::endl;
    }

    return;
}

template <typename ...Args>
void LogInfo(const char* fmt, Args... args) {
    mars::MarsLogger* logger = mars::MarsLogger::getInstance();

    bool Terminal, File;

    Terminal = logger->ifTerminalOutPut(mars::TerminalLogLevel::INFO);
    File = logger->ifFileOutPut(mars::FileLogLevel::INFO);

    std::cout << "Terminal: " << Terminal << " File: " << File << std::endl;

    if (!Terminal || !File) {
        return;
    }

    char buf[1024];
    snprintf(buf, sizeof(buf), fmt, args...);

    std::string log = logger->LogHead(mars::LogLevel::INFO, __FILE__, __func__, __LINE__) + buf;

    if (Terminal) {
        std::cout << log << std::endl; 
    }

    if (File) {
        logger->getFile() << log << std::endl;
    }

    return;
}

template <typename ...Args>
void LogWarn(const char* fmt, Args... args) {
    mars::MarsLogger* logger = mars::MarsLogger::getInstance();

    bool Terminal, File;

    Terminal = logger->ifTerminalOutPut(mars::TerminalLogLevel::WARN);
    File = logger->ifFileOutPut(mars::FileLogLevel::WARN);

    if (!Terminal || !File) {
        return;
    }

    char buf[1024];
    snprintf(buf, sizeof(buf), fmt, args...);

    std::string log = logger->LogHead(mars::LogLevel::WARN, __FILE__, __func__, __LINE__) + buf;

    if (Terminal) {
        std::cout << log << std::endl; 
    }

    if (File) {
        logger->getFile() << log << std::endl;
    }

    return;
}

template <typename ...Args>
void LogError(const char* fmt, Args... args) {
    mars::MarsLogger* logger = mars::MarsLogger::getInstance();

    bool Terminal, File;

    Terminal = logger->ifTerminalOutPut(mars::TerminalLogLevel::ERROR);
    File = logger->ifFileOutPut(mars::FileLogLevel::ERROR);

    if (!Terminal || !File) {
        return;
    }

    char buf[1024];
    snprintf(buf, sizeof(buf), fmt, args...);

    std::string log = logger->LogHead(mars::LogLevel::ERROR, __FILE__, __func__, __LINE__) + buf;

    if (Terminal) {
        std::cout << log << std::endl; 
    }

    if (File) {
        logger->getFile() << log << std::endl;
    }

    return;
}

template <typename ...Args>
void LogFatal(const char* fmt, Args... args) {
    mars::MarsLogger* logger = mars::MarsLogger::getInstance();

    bool Terminal, File;

    Terminal = logger->ifTerminalOutPut(mars::TerminalLogLevel::FATAL);
    File = logger->ifFileOutPut(mars::FileLogLevel::FATAL);

    if (!Terminal || !File) {
        return;
    }

    char buf[1024];
    snprintf(buf, sizeof(buf), fmt, args...);

    std::string log = logger->LogHead(mars::LogLevel::FATAL, __FILE__, __func__, __LINE__) + buf;

    if (Terminal) {
        std::cout << log << std::endl; 
    }

    if (File) {
        logger->getFile() << log << std::endl;
    }

    return;
}

#endif
