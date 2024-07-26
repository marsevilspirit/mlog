# mlog



mlog是一个用于C++程序服务的轻量，高性能日志库，可输出六种日志类型（Fatal，Error，Warn，Info，Debug，Trace），操作简单，支持高自定义输出类型，终端输出，文件输出，用`json`作为配置文件。

## 平台(Platforms)

* `Linux`(ok)
* `mac`(maybe)
* `windows`(maybe)

我编写的代码没有用到任何系统调用，应该支持跨平台



## 依赖

需要c++的fmt库，json库

archlinux：(使用yay进行下载)

```
$ yay fmt json
```

其他系统通过相关渠道下载即可



## 使用方法(Usage samples)

```c++
LogTrace("mlog is the NO.{}!", 10000000);
LogDebug("mlog is {}!", "best");
LogInfo("mlog is the NO.{}!", 10000000);
LogWarn("mlog is the NO.{}!", 10000000);
LogError("mlog is the NO.{}!", 10000000);
LogFatal("mlog is the NO.{}!", 10000000);
```

## 性能测试

```c++
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
```





## 目前可能面临的问题

- 在多线程环境下可能线程不安全
- 终端日志不会等待文件日志结束就会终止程序(已发现)
- 无颜色差别，不好分清不同的日志
- 尚未实现限制文件日志大小

- 尚未实现文件日志滚动

- 尚未实现多种格式化输入(本人感觉在我的代码里想通过重载<<来输出，是一件不可能的事情)

  
