#ifndef __LOGGING_H__
#define __LOGGING_H__
#include "Logging.h"
#include "CurrentThead.h"
#include "Thread.h"
#include "AsyncLogging.h"
#include "LogStream.h"
#include <assert.h>
#include <iostream>
#include <time.h>
#include <sys/time.h>

class Logger{
public:
    Logger(const char*filename,int line);
    ~Logger();
    LogStream&stream(){ return impl_.stream_; }

    static void setLogFileName(std::string filename) { logFileName_ = filename; }
    static std::string getLogFileName() { return logFileName_; }

private:
    class Impl{
        public:
            Impl(const char*fileName,int line);
            void formatTime();

            LogStream stream_;
            int line_;
            std::string basename_;
    };
    Impl impl_;
    static std::string logFileName_;
};

#define LOG Logger(__FILE__, __LINE__).stream()
#endif
