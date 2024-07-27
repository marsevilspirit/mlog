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
#define MARS_LOGGER_CALL

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
    void initLogConfig();
    void releaseLogConfig();
    std::string LogHead(LogLevel lvl, const char *file_name, const char *func_name, int line_no);
    bool ifFileOutPut(FileLogLevel fileLogLevel);
    bool ifTerminalOutPut(TerminalLogLevel terminalLogLevel);
    std::string getLogFileNameTime();
    std::string getLogOutPutTime();
    std::string getLogFileName() {
        return loggerConfig.logFileName;
    }
    
    std::ofstream& getFile() {
        return file;
    }

    bool LoggerStart() {
        return loggerConfig.logSwitch;
    }

    static MarsLogger* getInstance();

    void bindFileOutPutLevelMap(std::string json_value, FileLogLevel fileLogLevel);
    void bindTerminalOutPutLevelMap(std::string json_value, TerminalLogLevel terminalLogLevel);

    bool createFile (std::string path);

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
    static MarsLogger* single_instance;
    static std::mutex* mutex_new;
    std::map<FileLogLevel, bool> fileCoutMap;
    std::map<TerminalLogLevel, bool> terminalCoutMap;
    std::ostringstream stream;
    std::ofstream file;

    friend class LoggerMacros;

    MarsLogger();
    ~MarsLogger();

public:
    template <typename ...Args>
    void _log_trace(const char* fmt, const char* file_name, const char* func_name, int line_no, Args... args) {
        log_impl("TRACE", fmt, file_name, func_name, line_no, args...);
    }

    template <typename ...Args>
    void _log_debug(const char* fmt, const char* file_name, const char* func_name, int line_no, Args... args) {
        log_impl("DEBUG", fmt, file_name, func_name, line_no, args...);
    }

    template <typename ...Args>
    void _log_info(const char* fmt, const char* file_name, const char* func_name, int line_no, Args... args) {
        log_impl("INFO", fmt, file_name, func_name, line_no, args...);
    }

    template <typename ...Args>
    void _log_warn(const char* fmt, const char* file_name, const char* func_name, int line_no, Args... args) {
        log_impl("WARN", fmt, file_name, func_name, line_no, args...);
    }

    template <typename ...Args>
    void _log_error(const char* fmt, const char* file_name, const char* func_name, int line_no, Args... args) {
        log_impl("ERROR", fmt, file_name, func_name, line_no, args...);
    }

    template <typename ...Args>
    void _log_fatal(const char* fmt, const char* file_name, const char* func_name, int line_no, Args... args) {
        log_impl("FATAL", fmt, file_name, func_name, line_no, args...);
    }

    template <typename ...Args>
    void _log_impl(const char* level, const char* fmt, const char* file_name, const char* func_name, int line_no, Args... args) {
        if (!LoggerStart()) {
            return;
        }

        bool Terminal = ifTerminalOutPut(static_cast<TerminalLogLevel>(getLogLevel(level)));
        bool File = ifFileOutPut(static_cast<FileLogLevel>(getLogLevel(level)));

        if (!Terminal && !File) {
            return;
        }

        std::string log = LogHead(getLogLevel(level), file_name, func_name, line_no);
        log = log + fmt::format(fmt, args...);

        if (Terminal) {
            std::cout << log << '\n'; 
        }

        if (File) {
            getFile() << log << '\n';
        }
    }

    LogLevel getLogLevel(const char* level) {
        if (strcmp(level, "TRACE") == 0) return LogLevel::TRACE;
        if (strcmp(level, "DEBUG") == 0) return LogLevel::DEBUG;
        if (strcmp(level, "INFO") == 0) return LogLevel::INFO;
        if (strcmp(level, "WARN") == 0) return LogLevel::WARN;
        if (strcmp(level, "ERROR") == 0) return LogLevel::ERROR;
        if (strcmp(level, "FATAL") == 0) return LogLevel::FATAL;
        return LogLevel::INFO;
    }
};

}

#define LogInfo(fmt, ...)   mars::MarsLogger::getInstance()->_log_info(fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define LogWarn(fmt, ...)   mars::MarsLogger::getInstance()->_log_warn(fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define LogError(fmt, ...)  mars::MarsLogger::getInstance()->_log_error(fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define LogFatal(fmt, ...)  mars::MarsLogger::getInstance()->_log_fatal(fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define LogDebug(fmt, ...)  mars::MarsLogger::getInstance()->_log_debug(fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define LogTrace(fmt, ...)  mars::MarsLogger::getInstance()->_log_trace(fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__)


#endif
