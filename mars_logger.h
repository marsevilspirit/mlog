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

    std::string getLogFileNameTime ();

    static MarsLogger * getInstance ();

    void bindFileOutPutLevelMap (std::string json_value, FileLogLevel fileLogLevel);

    void bindTerminalOutPutLevelMap (std::string json_value, TerminalLogLevel terminalLogLevel);

    bool createFile (std::string path);

private:
    LoggerConfig loggerConfig;
    static MarsLogger * single_instance;
    static std::mutex * mutex_new;
    std::map<FileLogLevel, bool> fileCoutMap;
    std::map<TerminalLogLevel, bool> terminalCoutMap;
};

}

#endif
