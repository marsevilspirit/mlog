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
    std::ifstream input(LOG_CONFIG_PATH);
    if (!input) {
        std::cerr << "Unable to find the log configuration file, please make sure the file path is correct" << std::endl;
        return;
    }

    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(input, root, false)) {
        std::cerr << "parse log config file failed" << std::endl;
        return;
    }

    loggerConfig.logSwitch         = root["logSwitch"].asBool();
    loggerConfig.logTerminalSwitch = root["logTerminalSwitch"].asBool();
    loggerConfig.logTerminalLevel  = root["logTerminalLevel"].asString();
    loggerConfig.logFileSwitch     = root["logFileSwitch"].asBool();
    loggerConfig.logFileLevel      = root["logFileLevel"].asString();
    loggerConfig.logFileName       = root["logFileName"].asString() + getLogFileNameTime() + ".log";
    loggerConfig.logFilePath       = root["logFilePath"].asString();
    loggerConfig.details           = root["details"].asBool();
    loggerConfig.time              = root["time"].asBool();

    bindFileOutPutLevelMap(loggerConfig.logFileLevel);
    bindTerminalOutPutLevelMap(loggerConfig.logTerminalLevel);

    if (loggerConfig.logSwitch && loggerConfig.logFileSwitch) {
        if (!createFile(loggerConfig.logFilePath, loggerConfig.logFileName)) {
            std::cout << "Log work path creation failed\n";
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
    fileCoutMap[LogLevel::TRACE] = levels.find("5") != std::string::npos;
    fileCoutMap[LogLevel::DEBUG] = levels.find("4") != std::string::npos;
    fileCoutMap[LogLevel::INFO]  = levels.find("3") != std::string::npos;
    fileCoutMap[LogLevel::WARN]  = levels.find("2") != std::string::npos;
    fileCoutMap[LogLevel::ERROR] = levels.find("1") != std::string::npos;
    fileCoutMap[LogLevel::FATAL] = levels.find("0") != std::string::npos;
}

void MarsLogger::bindTerminalOutPutLevelMap(const std::string& levels) {
    terminalCoutMap[LogLevel::TRACE] = levels.find("5") != std::string::npos;
    terminalCoutMap[LogLevel::DEBUG] = levels.find("4") != std::string::npos;
    terminalCoutMap[LogLevel::INFO]  = levels.find("3") != std::string::npos;
    terminalCoutMap[LogLevel::WARN]  = levels.find("2") != std::string::npos;
    terminalCoutMap[LogLevel::ERROR] = levels.find("1") != std::string::npos;
    terminalCoutMap[LogLevel::FATAL] = levels.find("0") != std::string::npos;
}
