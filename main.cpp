#include <iostream>
#include <fstream>
#include <json/json.h>
#include "mars_logger.h"

int main()
{
    std::ifstream input("./logconf.json");
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(input, root, false)) {
        std::cerr << "parse log config file failed" << std::endl;
        return 0;
    }

    mars::MarsLogger* logger = mars::MarsLogger::getInstance();

    LogInfo("dfsadfsadf %d", 123);
}
