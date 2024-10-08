mlog是一个用于C++程序服务的轻量，高性能日志库，可输出六种日志类型（Fatal，Error，Warn，Info，Debug，Trace），操作简单，支持高自定义输出类型，终端输出，文件输出，用环境变量配置。



#### 平台支持

* `Linux`(ok)
* `mac`(maybe)
* `windows`(maybe)

我编写的代码没有用到任何系统调用，应该支持跨平台



#### 依赖

需要c++的fmt库

archlinux：(使用yay进行下载)

```
$ sudo pacman -S fmt
```

ubuntu:

```
sudo apt install libfmt-dev
```



其他系统通过相关渠道下载即可

mlog需要配置相应的环境变量

#### 环境变量配置

Level从F,E,W,I,D,T依次是`Fatal，Error，Warn，Info，Debug，Trace`

```
#日志开关
export MLOG_SWITCH=TRUE
#日志终端输出开关
export MLOG_TERMINAL_SWITCH=TRUE
#日志终端输出级别设置
export MLOG_TERMINAL_LEVEL=TDIWEF
#日志文件输出开关
export MLOG_FILE_SWITCH=FALSE
#日志文件输出级别设置
export MLOG_FILE_LEVEL=TDIWEF
#日志文件log name
export MLOG_FILE_NAME=log_name
#日志文件log path
export MLOG_FILE_PATH=/path/to/
#日志文件log details显示开关
export MLOG_DETAILS=TRUE
#日志文件log time显示开关
export MLOG_TIME=TRUE
```



#### 编译命令

```
g++ test.cpp mars_logger.cc -o logger.out -lfmt
```



#### 使用方法

无需任何初始化，直接调用即可

为了追求极简性，我只写了一个格式化方法，但也绝对够用，可以输出任何类型。

```c++
LogTrace("mlog is the NO.{}!", 10000000)
LogDebug("mlog is {}!", "best")
LogInfo("mlog is the NO.{}!", 10000000)
LogWarn("mlog is the NO.{}!", 10000000)
LogError("mlog is the NO.{}!", 10000000)
LogFatal("mlog is the NO.{}!", 10000000)
LogInfo("mlog is the NO.10000000")
```



#### 性能测试(details和time全部开启的情况下)

```c++
#include "mars_logger.h"

int main()
{
    time_t begin, end;
    double ret;
    begin = clock();
    std::string name = "mars";
    for(int i = 0; i< 10000; i++){
        LogFatal("我是{}{}", name, i)
        LogError("我是{}", name)
        LogWarn("我是{}", name)
        LogInfo("我是{}", name)
        LogDebug("我是{}", name)
        LogTrace("我是{}", name)
    }
    end = clock();
    ret = double(end - begin) / CLOCKS_PER_SEC;
    std::cout << "runtime:   " << ret << std::endl;
    
    return 0;
}
```

每个测试经过5次测试平均算出：

|       输出类型       | 时间       |
| :------------------: | ---------- |
|     只输出到终端     | 0.0888054s |
|     只输出到文件     | 0.0455120s |
| 同时输出到文件和终端 | 0.1061310s |



#### 目前可能面临的问题

- 在多线程环境下可能线程不安全

- 尚未实现限制文件日志大小

- 尚未实现文件日志滚动

- 尚未实现多种格式化输入(感觉在本人这段代码里想通过重载`<<`来输出，是一件不可能的事情)

- 由于我C++高级写法不精，无法想出将类函数隔离的方法，导致可以跳过宏函数直接访问类函数，只好在每个内部函数名前加上`_`下划线，但还是有一定隐患(说实话我想了很久，感觉也是件不可能的事)

  

#### 后期的优化方向

- 可以尝试异步进行终端和文件输出
- 目前只能输出常规类型，无法输出自定义类型
- 解决在终端彩色输出，文件中有彩色乱码的问题
