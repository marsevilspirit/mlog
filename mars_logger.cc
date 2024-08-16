#include "mars_logger.h"

using namespace mars;

std::unique_ptr<MarsLogger> MarsLogger::single_instance = nullptr;
std::mutex MarsLogger::mtx;

MarsLogger* MarsLogger::getInstance() {
    if (!single_instance) { 
        std::lock_guard<std::mutex> lock(mtx); 
        if (!single_instance) { 
            single_instance.reset(new MarsLogger()); 
        }
    }
    return single_instance.get(); 
}

MarsLogger::MarsLogger () {
    initLogConfig();
}

MarsLogger::~MarsLogger () {
    if (output_file.is_open()) {
        output_file.close();
    }
}

void MarsLogger::initLogConfig () {
    loggerConfig.logSwitch         = (std::getenv("MLOG_SWITCH") && std::string(std::getenv("MLOG_SWITCH")) == "TRUE") ? true : false;
    loggerConfig.logTerminalSwitch = (std::getenv("MLOG_TERMINAL_SWITCH") && std::string(std::getenv("MLOG_TERMINAL_SWITCH")) == "TRUE") ? true : false;
    loggerConfig.logTerminalLevel  = (std::getenv("MLOG_TERMINAL_LEVEL")) ? std::string(std::getenv("MLOG_TERMINAL_LEVEL")) : "";
    loggerConfig.logFileSwitch     = (std::getenv("MLOG_FILE_SWITCH") && std::string(std::getenv("MLOG_FILE_SWITCH")) == "TRUE") ? true : false;
    loggerConfig.logFileLevel      = (std::getenv("MLOG_FILE_LEVEL")) ? std::string(std::getenv("MLOG_FILE_LEVEL")) : "";
    loggerConfig.logFileName       = (std::getenv("MLOG_FILE_NAME") && std::string(std::getenv("MLOG_FILE_NAME")) != "") ? std::string(std::getenv("MLOG_FILE_NAME")) + getLogFileNameTime() + ".log" : "";
    loggerConfig.logFilePath       = (std::getenv("MLOG_FILE_PATH")) ? std::string(std::getenv("MLOG_FILE_PATH")) : "";
    loggerConfig.details           = (std::getenv("MLOG_DETAILS") && std::string(std::getenv("MLOG_DETAILS")) == "TRUE") ? true : false;
    loggerConfig.time              = (std::getenv("MLOG_TIME") && std::string(std::getenv("MLOG_TIME")) == "TRUE") ? true : false;

    bindFileOutPutLevelMap(loggerConfig.logFileLevel);
    bindTerminalOutPutLevelMap(loggerConfig.logTerminalLevel);

    if (loggerConfig.logSwitch && loggerConfig.logFileSwitch) {
        if (!createFile(loggerConfig.logFilePath, loggerConfig.logFileName)) {
            std::cerr << "Log work path creation failed\n";
        }
    }

    return;
}

std::string MarsLogger::LogHead (LogLevel lvl) {
    if (!loggerConfig.time) {
        return fmt::format("[{:5}] ", getLogLevelStr(lvl));
    }

    return fmt::format("[{}][{:5}] ", getLogOutPutTime(), getLogLevelStr(lvl));
}

std::string MarsLogger::LogDetail(const char *file_name, const char *func_name, int line_no) {
    if (!loggerConfig.details) {
        return "";
    }

    return fmt::format(" - [{} {}:{}]", file_name, func_name, line_no);
}

bool MarsLogger::createFile(const std::string& path, const std::string& fileName) {
    namespace fs = std::filesystem;
    try {
        // 创建完整的文件路径
        fs::path logFilePath = fs::path(path) / fileName;
        fs::path parent_path = logFilePath.parent_path();

        // 如果目录不存在，创建目录
        if (!parent_path.empty() && !fs::exists(parent_path)) {
            std::cout << "Creating directories: " << parent_path << std::endl;
            if (!fs::create_directories(parent_path)) {
                std::cerr << "Failed to create directories: " << parent_path << std::endl;
                return false;
            }
        }

        // 打开文件
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

bool MarsLogger::ifFileOutPut (LogLevel file_log_level) {
    return loggerConfig.logFileSwitch && fileCoutMap[file_log_level];
}

bool MarsLogger::ifTerminalOutPut (LogLevel terminal_log_level) {
    return loggerConfig.logTerminalSwitch && terminalCoutMap[terminal_log_level];
}

//得到log文件名的时间部分
std::string MarsLogger::getLogFileNameTime() {
    std::time_t now = std::time(nullptr);
    std::tm tm_buf;
    localtime_r(&now, &tm_buf);

    std::array<char, 20> timeString;
    strftime(timeString.data(), timeString.size(), "%Y-%m-%d-%H:%M:%S", &tm_buf);
    return std::string(timeString.data());
}

std::string MarsLogger::getLogOutPutTime() {
    std::time_t now = std::time(nullptr);
    std::tm tm_buf;
    localtime_r(&now, &tm_buf);

    std::array<char, 20> timeString;
    strftime(timeString.data(), timeString.size(), "%Y-%m-%d %H:%M:%S", &tm_buf);
    return std::string(timeString.data());
}

void MarsLogger::bindFileOutPutLevelMap(const std::string& levels) {
    fileCoutMap[LogLevel::TRACE] = levels.find("T") != std::string::npos;
    fileCoutMap[LogLevel::DEBUG] = levels.find("D") != std::string::npos;
    fileCoutMap[LogLevel::INFO]  = levels.find("I") != std::string::npos;
    fileCoutMap[LogLevel::WARN]  = levels.find("W") != std::string::npos;
    fileCoutMap[LogLevel::ERROR] = levels.find("E") != std::string::npos;
    fileCoutMap[LogLevel::FATAL] = levels.find("F") != std::string::npos;
}

void MarsLogger::bindTerminalOutPutLevelMap(const std::string& levels) {
    terminalCoutMap[LogLevel::TRACE] = levels.find("T") != std::string::npos;
    terminalCoutMap[LogLevel::DEBUG] = levels.find("D") != std::string::npos;
    terminalCoutMap[LogLevel::INFO]  = levels.find("I") != std::string::npos;
    terminalCoutMap[LogLevel::WARN]  = levels.find("W") != std::string::npos;
    terminalCoutMap[LogLevel::ERROR] = levels.find("E") != std::string::npos;
    terminalCoutMap[LogLevel::FATAL] = levels.find("F") != std::string::npos;
}
