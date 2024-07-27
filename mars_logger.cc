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
    if (file.is_open()) {
        file.close();
    }
}

void MarsLogger::initLogConfig () {
    std::ifstream input(LOG_CONFIG_PATH);
    if (!input) {
        std::cerr << "Failed to open log config file" << std::endl;
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

    bindFileOutPutLevelMap(loggerConfig.logFileLevel);
    bindTerminalOutPutLevelMap(loggerConfig.logTerminalLevel);

    if (loggerConfig.logSwitch && loggerConfig.logFileSwitch) {
        if (!createFile(loggerConfig.logFilePath)) {
            std::cout << "Log work path creation failed\n";
        }
    }

    return;
}

std::string MarsLogger::LogHead (LogLevel lvl, const char *file_name, const char *func_name, int line_no) {
    return fmt::format("[{} {} {}:{}] {:5} ", getLogOutPutTime(), file_name, func_name, line_no, getLogLevelStr(lvl));
}

bool MarsLogger::createFile (std::string path) {
    namespace fs = std::filesystem;    
    try {
        if (fs::exists(loggerConfig.logFileName)) {
            std::cerr << "File already exists: " << path << std::endl;
            return false;
        }

        fs::path parent_path = fs::path(path).parent_path();

        if (!parent_path.empty() && !fs::exists(parent_path)) {
            std::cout << "你指定的log文件路径不存在，正在创建\n";
            if (!fs::create_directories(parent_path)) {
                std::cerr << "Failed to create directories: " << parent_path << std::endl;
                return false;
            }
        }
        
        file = std::ofstream(loggerConfig.logFileName);
        if (!file) {
            std::cerr << "Failed to create file: " << path << std::endl;
            return false;
        }        

        return true;
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return false;
    }
}

bool MarsLogger::ifFileOutPut (LogLevel file_log_level) {
    return fileCoutMap[file_log_level] && loggerConfig.logFileSwitch;
}

bool MarsLogger::ifTerminalOutPut (LogLevel terminal_log_level) {
    return terminalCoutMap[terminal_log_level] && loggerConfig.logTerminalSwitch;
}

//得到log文件名的时间部分
std::string MarsLogger::getLogFileNameTime() {
    std::time_t time = std::time(nullptr);
    char timeString[20];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d-%H:%M:%S",localtime(&time));
    return timeString;
}

std::string MarsLogger::getLogOutPutTime() {
    std::time_t time = std::time(nullptr);
    char timeString[20];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S",localtime(&time));
    return timeString;
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
