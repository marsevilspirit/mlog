#include "mars_logger.h"

using namespace mars;

std::unique_ptr<MarsLogger> MarsLogger::single_instance = nullptr;
std::mutex MarsLogger::mtx;

MarsLogger* MarsLogger::getInstance() {
    static MarsLogger instance;
    return &instance;
}

MarsLogger::MarsLogger() {
    initLogConfig();
}

MarsLogger::~MarsLogger() {
    if (output_file.is_open()) {
        output_file.close();
    }
}

static bool getEnvFlag(const char* env_var) {
    const char* env_value = std::getenv(env_var);
    return env_value && std::string(env_value) == "TRUE";
}

static std::string getEnvOrDefault(const char* env_var, const std::string& default_value) {
    const char* env_value = std::getenv(env_var);
    return env_value ? std::string(env_value) : default_value;
}

void MarsLogger::initLogConfig() {
    loggerConfig.logSwitch = getEnvFlag("MLOG_SWITCH");
    loggerConfig.logTerminalSwitch = getEnvFlag("MLOG_TERMINAL_SWITCH");
    loggerConfig.logTerminalLevel = getEnvOrDefault("MLOG_TERMINAL_LEVEL", "");
    loggerConfig.logFileSwitch = getEnvFlag("MLOG_FILE_SWITCH");
    loggerConfig.logFileLevel = getEnvOrDefault("MLOG_FILE_LEVEL", "");
    loggerConfig.logFileName = getEnvOrDefault("MLOG_FILE_NAME", "") + getLogFileNameTime() + ".log";
    loggerConfig.logFilePath = getEnvOrDefault("MLOG_FILE_PATH", "");
    loggerConfig.details = getEnvFlag("MLOG_DETAILS");
    loggerConfig.time = getEnvFlag("MLOG_TIME");

    bindFileOutPutLevelMap(loggerConfig.logFileLevel);
    bindTerminalOutPutLevelMap(loggerConfig.logTerminalLevel);

    if (loggerConfig.logSwitch && loggerConfig.logFileSwitch) {
        if (!createFile(loggerConfig.logFilePath, loggerConfig.logFileName)) {
            std::cerr << "Log work path creation failed\n";
        }
    }
}

std::string MarsLogger::LogHead(LogLevel lvl) {
    if (!loggerConfig.time) {
        return fmt::format("[{:5}] ", getLogLevelStr(lvl));
    }
    return fmt::format("[{}][{:5}] ", getLogOutPutTime(), getLogLevelStr(lvl));
}

std::string MarsLogger::LogDetail(const char* file_name, const char* func_name, int line_no) {
    if (!loggerConfig.details) {
        return "";
    }
    return fmt::format(" - [{} {}:{}]", file_name, func_name, line_no);
}

bool MarsLogger::createFile(const std::string& path, const std::string& fileName) {
    try {
        std::filesystem::path logFilePath = std::filesystem::path(path) / fileName;
        std::filesystem::create_directories(logFilePath.parent_path());

        output_file.open(logFilePath);
        if (!output_file.is_open()) {
            std::cerr << "Failed to create file: " << logFilePath << std::endl;
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return false;
    }
}

bool MarsLogger::ifFileOutPut(LogLevel file_log_level) {
    return loggerConfig.logFileSwitch && fileCoutMap[file_log_level];
}

bool MarsLogger::ifTerminalOutPut(LogLevel terminal_log_level) {
    return loggerConfig.logTerminalSwitch && terminalCoutMap[terminal_log_level];
}

static std::string getCurrentTime(const char* format) {
    std::time_t now = std::time(nullptr);
    std::tm tm_buf;
    localtime_r(&now, &tm_buf);

    std::array<char, 20> timeString;
    strftime(timeString.data(), timeString.size(), format, &tm_buf);
    return std::string(timeString.data());
}

std::string MarsLogger::getLogFileNameTime() {
    return getCurrentTime("%Y-%m-%d-%H:%M:%S");
}

std::string MarsLogger::getLogOutPutTime() {
    return getCurrentTime("%Y-%m-%d %H:%M:%S");
}

void MarsLogger::bindFileOutPutLevelMap(const std::string& levels) {
    fileCoutMap[LogLevel::TRACE] = levels.find('T') != std::string::npos;
    fileCoutMap[LogLevel::DEBUG] = levels.find('D') != std::string::npos;
    fileCoutMap[LogLevel::INFO]  = levels.find('I') != std::string::npos;
    fileCoutMap[LogLevel::WARN]  = levels.find('W') != std::string::npos;
    fileCoutMap[LogLevel::ERROR] = levels.find('E') != std::string::npos;
    fileCoutMap[LogLevel::FATAL] = levels.find('F') != std::string::npos;
}

void MarsLogger::bindTerminalOutPutLevelMap(const std::string& levels) {
    terminalCoutMap[LogLevel::TRACE] = levels.find('T') != std::string::npos;
    terminalCoutMap[LogLevel::DEBUG] = levels.find('D') != std::string::npos;
    terminalCoutMap[LogLevel::INFO]  = levels.find('I') != std::string::npos;
    terminalCoutMap[LogLevel::WARN]  = levels.find('W') != std::string::npos;
    terminalCoutMap[LogLevel::ERROR] = levels.find('E') != std::string::npos;
    terminalCoutMap[LogLevel::FATAL] = levels.find('F') != std::string::npos;
}
