#ifndef MARS_LOGGER_H_
#define MARS_LOGGER_H_

#include <iostream>
#include <string>
#include <fstream>
#include <mutex>
#include <filesystem>
#include <fmt/core.h>
#include <unordered_map>
#include <array>
#include <cstdlib>

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
    bool ifFileOutPut(LogLevel fileLogLevel);
    bool ifTerminalOutPut(LogLevel terminalLogLevel);
    std::string getLogFileNameTime();
    std::string getLogOutPutTime();
    std::string getLogFileName() {return loggerConfig.logFileName;}
    static MarsLogger* getInstance();
    std::string LogHead(LogLevel lvl);
    std::string LogDetail(const char *file_name, const char *func_name, int line_no);
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

        std::string log;

        try{
            log = LogHead(level) + fmt::format(fmt, args...) + LogDetail(file_name, func_name, line_no);
        } catch (const std::exception& e) {
            std::cerr << "\033[31mError in log format: " << e.what() << " in " << file_name << " at " << func_name << " line " << line_no << "\033[0m\n";
            return;
        }

        {
            std::lock_guard<std::mutex> lock(log_mutex); // 加锁以确保线程安全

            if (Terminal) {
                std::cout << colorizeLog(level, log) << '\n'; 
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
        {LogLevel::FATAL, "FATAL"},
        {LogLevel::ERROR, "ERROR"},
        {LogLevel::WARN, "WARN "},
        {LogLevel::INFO, "INFO "},
        {LogLevel::DEBUG, "DEBUG"},
        {LogLevel::TRACE, "TRACE"}
    };

    MarsLogger();

    // 添加颜色到日志文本
    std::string colorizeLog(LogLevel level, const std::string& log) {
        switch (level) {
            case LogLevel::FATAL:
            case LogLevel::ERROR:
                return RED + log + RESET;
            case LogLevel::WARN:
                return YELLOW + log + RESET;
            case LogLevel::INFO:
            case LogLevel::DEBUG:
            case LogLevel::TRACE:
                return WHITE + log + RESET;
            default:
                return log;
        }
    }
};

}

#define LogInfo(fmt, ...)   mars::MarsLogger::getInstance()->_log_impl(mars::LogLevel::INFO,  fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__);
#define LogWarn(fmt, ...)   mars::MarsLogger::getInstance()->_log_impl(mars::LogLevel::WARN,  fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__);
#define LogError(fmt, ...)  mars::MarsLogger::getInstance()->_log_impl(mars::LogLevel::ERROR, fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__);
#define LogFatal(fmt, ...)  mars::MarsLogger::getInstance()->_log_impl(mars::LogLevel::FATAL, fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__);
#define LogDebug(fmt, ...)  mars::MarsLogger::getInstance()->_log_impl(mars::LogLevel::DEBUG, fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__);
#define LogTrace(fmt, ...)  mars::MarsLogger::getInstance()->_log_impl(mars::LogLevel::TRACE, fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__);

#endif
