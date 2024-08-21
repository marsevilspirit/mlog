export MLOG_SWITCH=TRUE
export MLOG_TERMINAL_SWITCH=TRUE
export MLOG_TERMINAL_LEVEL=TDIWEF
export MLOG_FILE_SWITCH=FALSE
export MLOG_FILE_LEVEL=TDIWEF
export MLOG_FILE_NAME=test_log
export MLOG_FILE_PATH=/home/mars/code/cpp_log/
export MLOG_DETAILS=TRUE
export MLOG_TIME=TRUE

clang++ test.cpp mars_logger.cc -o logger.out -lfmt
