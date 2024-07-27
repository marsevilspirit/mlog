#include "mars_logger.h"

int main()
{
    time_t begin, end;
    double ret;
    begin = clock();
    std::string name = "mars";
    for(int i = 0; i< 10000; i++){
        LogError("我是{} {}", name, i);
        LogWarn("我是{}", name);
        LogInfo("我是{}", name);
        LogDebug("我是{}", name);
        LogTrace("我是{}", name);
    }
    end = clock();
    ret = double(end - begin) / CLOCKS_PER_SEC;
    std::cout << "runtime:   " << ret << std::endl;
    
    return 0;
}
