#include <iostream>
#include <fstream>
#include <json/json.h>

int main()
{
    std::ifstream input("./logconf.json");
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(input, root, false)) {
        std::cerr << "parse log config file failed" << std::endl;
        return 0;
    }

    std::cout << root["logSwitch"].asBool() << '\n';
    std::cout << root["logFileSwitch"].asBool() << '\n';
    std::cout << root["logTerminalSwitch"].asBool() << '\n';
    std::cout << root["logTerminalLevel"].asString() << '\n';
    std::cout << root["logFileLevel"].asString() << '\n';
    std::cout << root["logFileName"].asString() << '\n';
    std::cout << root["logFilePath"].asString() << '\n';
    std::cout << root["logFileMaxSize"].asString() << '\n';
    std::cout << root["logFileReachMaxBehavior"].asString() << '\n'; 
}
