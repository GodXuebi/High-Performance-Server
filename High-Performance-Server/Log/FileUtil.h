#ifndef __FILEUTIL_H__
#define __FILEUTIL_H__

#include "noncopyable.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>

/*------------------------------------------
AppendFile Class only requires the path of where to store a logfile, and opens automatically 
a file descriptor with a buffer.
------------------------------------------*/

class AppendFile : public noncopyable{
public:
    explicit AppendFile(std::string filename);
    ~AppendFile();
    void append(const char*logline,const size_t len);
    void flush();
private:
    FILE*fp;
    char buffer[64*1024];
    size_t write(const char*logline,const size_t len);
};

#endif