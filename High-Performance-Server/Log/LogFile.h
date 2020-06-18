#ifndef __LOGFILE_H__
#define __LOGFILE_H__
#include <memory>
#include <string>
#include "FileUtil.h"
#include "MutexLock.h"
#include "noncopyable.h"


/*----------------------------------------------------
LogFile Class only realizes the safety of multithread with lock and also realizes the flush
every N times.
----------------------------------------------------*/

class LogFile : public noncopyable{
public:
    LogFile(const std::string&basename_,int flushEveryN_ = 1024);
    ~LogFile();
    
    void append(const char* logline,const int len);
    void flush();
private:
    void append_unlocked(const char* logline,const int len);
    const std::string basename;
    const int flushEveryN;

    int count;
    std::unique_ptr<MutexLock> mutex;
    std::unique_ptr<AppendFile> file;
};


#endif