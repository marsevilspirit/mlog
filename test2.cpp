#include "mars_logger.h"

int main()
{
    // 初始化日志, 获取单例
    mars::MarsLogger* logger = mars::MarsLogger::getInstance();

    time_t begin, end;
    double ret;
    begin = clock();
    std::string name = "mars";

    LogInfo("good");

    end = clock();
    ret = double(end - begin) / CLOCKS_PER_SEC;
    std::cout << "runtime:   " << ret << std::endl;
    
    return 0;
}
