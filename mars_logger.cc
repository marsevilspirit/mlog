#include "mars_logger.h"
#include <fstream>

using namespace mars;

MarsLogger * MarsLogger::single_instance = nullptr;
std::mutex * MarsLogger::mutex_new = new std::mutex();

//获得单例
MarsLogger* MarsLogger::getInstance () {
    mutex_new->lock(); 
    if (single_instance == nullptr) {
        single_instance = new MarsLogger();
    }
    mutex_new->unlock();
    return single_instance;
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

    loggerConfig.logSwitch = root["logSwitch"].asString();
    loggerConfig.logTerminalSwitch = root["logTerminalSwitch"].asString();
    loggerConfig.logTerminalLevel = root["logTerminalLevel"].asString();
    loggerConfig.logFileSwitch = root["logFileSwitch"].asString();
    loggerConfig.logFileLevel = root["logFileLevel"].asString();
    loggerConfig.logFileName = root["logFileName"].asString();
    loggerConfig.logFilePath = root["logFilePath"].asString();
    loggerConfig.logFileMaxSize = root["logFileMaxSize"].asString();
    loggerConfig.logFileReachMaxBehavior = root["logFileReachMaxBehavior"].asString(); 

    loggerConfig.logFileName = loggerConfig.logFileName + getLogFileNameTime() + ".log";

    bindFileOutPutLevelMap("5", FileLogLevel::TRACE);
    bindFileOutPutLevelMap("4", FileLogLevel::DEBUG);
    bindFileOutPutLevelMap("3", FileLogLevel::INFO);
    bindFileOutPutLevelMap("2", FileLogLevel::WARN);
    bindFileOutPutLevelMap("1", FileLogLevel::ERROR);
    bindFileOutPutLevelMap("0", FileLogLevel::FATAL);

    bindTerminalOutPutLevelMap("5", TerminalLogLevel::TRACE);
    bindTerminalOutPutLevelMap("4", TerminalLogLevel::DEBUG);
    bindTerminalOutPutLevelMap("3", TerminalLogLevel::INFO);
    bindTerminalOutPutLevelMap("2", TerminalLogLevel::WARN);
    bindTerminalOutPutLevelMap("1", TerminalLogLevel::ERROR);
    bindTerminalOutPutLevelMap("0", TerminalLogLevel::FATAL);

    if(loggerConfig.logFileSwitch == "on"){
        if(!createFile(loggerConfig.logFilePath)){
            std::cout<<"Log work path creation failed\n";
        }
    }

    std::cout << "loggerConfig.logSwitch: " << loggerConfig.logSwitch << std::endl;

    return;
}

std::string MarsLogger::LogHead(LogLevel lvl, const char *file_name, const char *func_name, int line_no) {
    std::string logLevelStr = getLogLevelStr(lvl);
    std::string logTime = getLogOutPutTime();
    std::string res = "[" + logTime + " " + file_name + " " + func_name + ":" + std::to_string(line_no) + "]" + " " + logLevelStr + " ";
    return res;
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

bool MarsLogger::ifFileOutPut (FileLogLevel file_log_level) {
    return fileCoutMap[file_log_level];
}

bool MarsLogger::ifTerminalOutPut (TerminalLogLevel terminal_log_level) {
    return terminalCoutMap[terminal_log_level];
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

void MarsLogger::bindFileOutPutLevelMap (std::string json_value, FileLogLevel file_log_level) {
    if(loggerConfig.logFileLevel.find(json_value) != std::string::npos)
        fileCoutMap[file_log_level] = true;
    else {
        fileCoutMap[file_log_level] = false;
    }
}

void MarsLogger::bindTerminalOutPutLevelMap (std::string json_value, TerminalLogLevel terminal_log_level) {
    if(loggerConfig.logTerminalLevel.find(json_value) != std::string::npos)
        terminalCoutMap[terminal_log_level] = true;
    else {
        terminalCoutMap[terminal_log_level] = false;
    }
}
