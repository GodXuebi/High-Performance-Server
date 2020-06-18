#include "LogFile.h"

LogFile::LogFile(const std::string&basename_,int flushEveryN_):
basename(basename_),flushEveryN(flushEveryN_),count(0),mutex(new MutexLock),
file(new AppendFile(basename)){}



void LogFile::flush()
{
    MutexLockGuard lock(*mutex);
    file->flush();
}

LogFile::~LogFile(){}

void LogFile::append_unlocked(const char*logline, const int len)
{
    file->append(logline,len);
    count++;
    if(count >= flushEveryN)
    {
        count = 0;
        file->flush();
    }
}

void LogFile::append(const char* logline,const int len)
{
    MutexLockGuard lock(*mutex);
    append_unlocked(logline,len);
}
