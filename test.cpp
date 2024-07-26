#include "mars_logger.h"

int main()
{
    mars::MarsLogger* logger = mars::MarsLogger::getInstance();

    time_t begin, end;
    double ret;
    begin = clock();
    std::string name = "mars";
    for(int i = 0; i< 10000; i++){
        mars::LogError("我是{} {}", name, i);
        mars::LogWarn("我是{}", name);
        mars::LogInfo("我是{}", name);
        mars::LogDebug("我是{}", name);
        mars::LogTrace("我是{}", name);
    }
    end = clock();
    ret = double(end-begin) / CLOCKS_PER_SEC;
    std::cout <<"runtime:   "<<ret<<std::endl;
    
    return 0;
}
