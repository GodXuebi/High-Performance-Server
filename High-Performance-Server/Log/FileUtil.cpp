#include "FileUtil.h"


AppendFile::AppendFile(std::string filename):fp(fopen(filename.c_str(),"ae"))
{// "ae" append + close the fp when using exec to build a new process
    setbuffer(fp,buffer,sizeof(buffer));
}

AppendFile::~AppendFile(){fclose(fp);}

size_t  AppendFile::write(const char*logline,const size_t len)
{
    return fwrite_unlocked(logline,len,1,fp);
}


void AppendFile::append(const char* logline, const size_t len) 
{
    size_t n = this->write(logline,len);
    size_t remain = len - n;
    while(remain > 0)
    {
        size_t x = this->write(logline + n,remain);
        if(x == 0)
        {
            int err = ferror(fp);
            if(err) fprintf(stderr,"AppendFile::append() failed !\n");
            break;
        }
        n += x;
        remain = len - n;
    }
}

void AppendFile::flush(){fflush(fp);}