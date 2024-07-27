mlog是一个用于C++程序服务的轻量，高性能日志库，可输出六种日志类型（Fatal，Error，Warn，Info，Debug，Trace），操作简单，支持高自定义输出类型，终端输出，文件输出，用`json`作为配置文件。



#### 平台支持

* `Linux`(ok)
* `mac`(maybe)
* `windows`(maybe)

我编写的代码没有用到任何系统调用，应该支持跨平台



#### 依赖

需要c++的fmt库，json库

archlinux：(使用yay进行下载)

```
$ yay fmt json
```

其他系统通过相关渠道下载即可

在本日志库的文件夹中，必须包含`logconf.json`的配置文件



#### 配置文件(命名为`logconf.json`)

Level从0到5依次是`Fatal，Error，Warn，Info，Debug，Trace`

```json
{
    "logSwitch": true,
    "logTerminalSwitch": true,
    "logTerminalLevel": "0,1,2,3,4,5",
    "logFileSwitch": false, 
    "logFileLevel": "0,1,2,3,4,5",
    "logFileName": "app_log",
    "logFilePath": "/home/mars/code/",
    "logFileMaxSize": "100",
    "logFileReachMaxBehavior": "nothing"
}
```



#### 编译命令

```
g++ test.cpp mars_logger.cc -ljsoncpp -lstdc++fs -o logger.out -lfmt
```



#### 使用方法

无需任何初始化，直接调用即可(你就说方便不方便)

```c++
LogTrace("mlog is the NO.{}!", 10000000);
LogDebug("mlog is {}!", "best");
LogInfo("mlog is the NO.{}!", 10000000);
LogWarn("mlog is the NO.{}!", 10000000);
LogError("mlog is the NO.{}!", 10000000);
LogFatal("mlog is the NO.{}!", 10000000);
LogInfo("mlog is the NO.10000000");
```



#### 性能测试

```c++
#include "mars_logger.h"

int main()
{
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
    ret = double(end - begin) / CLOCKS_PER_SEC;
    std::cout << "runtime:   " << ret << std::endl;
    
    return 0;
}
```



|       输出类型       | 时间      |
| :------------------: | --------- |
|     只输出到终端     | 0.102091s |
|     只输出到文件     | 0.085482s |
| 同时输出到文件和终端 | 0.109722s |



#### 目前可能面临的问题

- 在多线程环境下可能线程不安全

- 终端日志不会等待文件日志结束就会终止程序(已发现)

-  尚未实现限制文件日志大小

- 尚未实现文件日志滚动

- 尚未实现多种格式化输入(感觉在本人这段代码里想通过重载`<<`来输出，是一件不可能的事情)

- 由于我C++高级写法不精，无法想出将类函数隔离的方法，导致可以跳过宏函数直接访问类函数，只好在每个内部函数名前加上`_`下划线，但还是有一定隐患(说实话我想了很久，感觉也是件不可能的事)

  
