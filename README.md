# mlog

mlog是一个轻量化，目前低性能的C++日志库

日志分为六个级别，可通过json配置文件输出任意级别的日志



## Platforms

* Linux(ok)

* mac(maybe)

* windows(maybe)

  

  我编写没有用到任何系统调用，应该支持跨平台

## Usage samples：

```
LogTrace("mlog is the NO.%d!", 10000000);
LogDebug("mlog is %s!", "best");
LogInfo("mlog is the NO.%d!", 10000000);
LogWarn("mlog is the NO.%d!", 10000000);
LogError("mlog is the NO.%d!", 10000000);
LogFatal("mlog is the NO.%d!", 10000000);
```

