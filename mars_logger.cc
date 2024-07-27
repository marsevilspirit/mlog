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
    file.close();
}

void MarsLogger::initLogConfig () {
    std::ifstream input(LOG_CONFIG_PATH);
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

    bindFileOutPutLevelMap("5", LogLevel::TRACE);
    bindFileOutPutLevelMap("4", LogLevel::DEBUG);
    bindFileOutPutLevelMap("3", LogLevel::INFO);
    bindFileOutPutLevelMap("2", LogLevel::WARN);
    bindFileOutPutLevelMap("1", LogLevel::ERROR);
    bindFileOutPutLevelMap("0", LogLevel::FATAL);

    bindTerminalOutPutLevelMap("5", LogLevel::TRACE);
    bindTerminalOutPutLevelMap("4", LogLevel::DEBUG);
    bindTerminalOutPutLevelMap("3", LogLevel::INFO);
    bindTerminalOutPutLevelMap("2", LogLevel::WARN);
    bindTerminalOutPutLevelMap("1", LogLevel::ERROR);
    bindTerminalOutPutLevelMap("0", LogLevel::FATAL);

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
    char timeString[std::size("yyyy-mm-dd-hh:mm:ss")];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d-%H:%M:%S",localtime(&time));
    return timeString;
}

std::string MarsLogger::getLogOutPutTime() {
    std::time_t time = std::time(nullptr);
    char timeString[std::size("yyyy-mm-dd hh:mm:ss")];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S",localtime(&time));
    return timeString;
}

void MarsLogger::bindFileOutPutLevelMap (std::string json_value, LogLevel file_log_level) {
    if(loggerConfig.logFileLevel.find(json_value) != std::string::npos)
        fileCoutMap[file_log_level] = true;
    else {
        fileCoutMap[file_log_level] = false;
    }
}

void MarsLogger::bindTerminalOutPutLevelMap (std::string json_value, LogLevel terminal_log_level) {
    if(loggerConfig.logTerminalLevel.find(json_value) != std::string::npos)
        terminalCoutMap[terminal_log_level] = true;
    else {
        terminalCoutMap[terminal_log_level] = false;
    }
}
