#ifndef MARS_LOGGER_H_
#define MARS_LOGGER_H_

#include <iostream>
#include <string>
#include <json/json.h>
#include <fstream>
#include <mutex>
#include <filesystem>
#include <fmt/core.h>
#include <unordered_map>

#define LOG_CONFIG_PATH "./logconf.json"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"
#define WHITE "\033[37m"

namespace mars {

enum class LogLevel : int { FATAL = 0, ERROR, WARN, INFO, DEBUG, TRACE };

struct LoggerConfig {
    bool         logSwitch;
    bool         logTerminalSwitch;
    std::string  logTerminalLevel;
    bool         logFileSwitch; 
    std::string  logFileLevel;
    std::string  logFileName;
    std::string  logFilePath;
    bool         details;
    bool         time;
};

class MarsLogger {
public:
    void initLogConfig();
    std::string LogHead(LogLevel lvl, const char *file_name, const char *func_name, int line_no);
    bool ifFileOutPut(LogLevel fileLogLevel);
    bool ifTerminalOutPut(LogLevel terminalLogLevel);
    std::string getLogFileNameTime();
    std::string getLogOutPutTime();
    std::string getLogFileName() {return loggerConfig.logFileName;}
    static MarsLogger* getInstance();
    void bindFileOutPutLevelMap(const std::string& levels);
    void bindTerminalOutPutLevelMap(const std::string& levels);
    bool createFile(const std::string& path, const std::string& fileName);
    ~MarsLogger();

    template <typename T>
    std::string getLogLevelStr(T level) {
        auto it = logLevelMap.find(level);
        if (it != logLevelMap.end()) {
            return it->second;
        }
        return "UNKNOWN";
    }

    template <typename ...Args>
    void _log_impl(LogLevel level, const char* fmt, const char* file_name, const char* func_name, int line_no, Args... args) {
        if (!loggerConfig.logSwitch) {
            return;
        }

        bool Terminal = ifTerminalOutPut(level);
        bool File = ifFileOutPut(level);

        if (!Terminal && !File) {
            return;
        }

        std::string log = LogHead(level, file_name, func_name, line_no) + fmt::format(fmt, args...);

        {
            std::lock_guard<std::mutex> lock(log_mutex); // 加锁以确保线程安全

            if (Terminal) {
                std::cout << log << '\n'; 
            }

            if (File) {
                output_file << log << '\n';
            }
        }
    }

private:
    LoggerConfig loggerConfig;
    static std::unique_ptr<MarsLogger> single_instance;
    static std::mutex mtx;
     std::mutex log_mutex; // 保护日志输出的互斥量
    std::unordered_map<LogLevel, bool> fileCoutMap;
    std::unordered_map<LogLevel, bool> terminalCoutMap;
    std::ofstream output_file;
    std::unordered_map<LogLevel, std::string> logLevelMap {
        {LogLevel::FATAL, RED "FATAL" RESET},
        {LogLevel::ERROR, RED "ERROR" RESET},
        {LogLevel::WARN, YELLOW "WARN " RESET},
        {LogLevel::INFO, WHITE "INFO " RESET},
        {LogLevel::DEBUG, WHITE "DEBUG" RESET},
        {LogLevel::TRACE, WHITE "TRACE" RESET}
    };

    MarsLogger();
};

}

#define LogInfo(fmt, ...)   mars::MarsLogger::getInstance()->_log_impl(mars::LogLevel::INFO, WHITE fmt RESET, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define LogWarn(fmt, ...)   mars::MarsLogger::getInstance()->_log_impl(mars::LogLevel::WARN, YELLOW fmt RESET, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define LogError(fmt, ...)  mars::MarsLogger::getInstance()->_log_impl(mars::LogLevel::ERROR, RED fmt RESET, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define LogFatal(fmt, ...)  mars::MarsLogger::getInstance()->_log_impl(mars::LogLevel::FATAL, RED fmt RESET, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define LogDebug(fmt, ...)  mars::MarsLogger::getInstance()->_log_impl(mars::LogLevel::DEBUG, WHITE fmt RESET, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define LogTrace(fmt, ...)  mars::MarsLogger::getInstance()->_log_impl(mars::LogLevel::TRACE, WHITE fmt RESET, __FILE__, __func__, __LINE__, ##__VA_ARGS__)

#endif
