#include "Logging.h"

static pthread_once_t once_control_ = PTHREAD_ONCE_INIT;
static AsyncLogging*AsyncLogging_;


std::string Logger::logFileName_ = "./WebServer.log";

void once_init()
{
    AsyncLogging_ = new AsyncLogging(Logger::getLogFileName());
    AsyncLogging_->start();     
}


void output(const char*msg,const int len)
{
    pthread_once(&once_control_,once_init);
    AsyncLogging_->append(msg,len);
}

Logger::Impl::Impl(const char*filename,int line)
    :stream_(),
    line_(line),
    basename_(filename)
    {
        formatTime();
    }

void Logger::Impl::formatTime()
{
    struct timeval tv;
    time_t time;
    char str_t[26] = {0};
    gettimeofday(&tv,NULL);
    time = tv.tv_sec;
    struct tm*p_time = localtime(&time);
    strftime(str_t, 26, "%Y-%m-%d %H:%M:%S\n", p_time);
    stream_ << str_t;
}

Logger::Logger(const char*fileName,int line):impl_(fileName,line){}

Logger::~Logger()
{
    impl_.stream_ << " -- " << impl_.basename_ << ':' << impl_.line_ << '\n';
    const LogStream::Buffer& buf(stream().buffer());
    output(buf.data(), buf.length());
}

