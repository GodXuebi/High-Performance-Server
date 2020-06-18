#pragma once
#include "timer.h"
#include "util.h"
#include <string>
#include <unordered_map>
#include <memory>


const int STATE_PARSE_URI = 1;
const int STATE_PARSE_HEADERS = 2;
const int STATE_RECV_BODY = 3;
const int STATE_ANALYSIS = 4;
const int STATE_FINISH = 5;

const int MAX_BUFF = 4096;

// 有请求出现但是读不到数据,可能是Request Aborted,
// 或者来自网络的数据没有达到等原因,
// 对这样的请求尝试超过一定的次数就抛弃
const int AGAIN_MAX_TIMES = 200;

const int PARSE_URI_AGAIN = -1;
const int PARSE_URI_ERROR = -2;
const int PARSE_URI_SUCCESS = 0;

const int PARSE_HEADER_AGAIN = -1;
const int PARSE_HEADER_ERROR = -2;
const int PARSE_HEADER_SUCCESS = 0;

const int ANALYSIS_ERROR = -2;
const int ANALYSIS_SUCCESS = 0;

const int METHOD_POST = 1;
const int METHOD_GET = 2;
const int HTTP_10 = 1;
const int HTTP_11 = 2;

const int EPOLL_WAIT_TIME = 500;

class MimeType
{
private:
    static void init();
    static std::unordered_map<std::string, std::string> mime;
    MimeType();
    MimeType(const MimeType &m);

public:
    static std::string getMime(const std::string &suffix);

private:
    static pthread_once_t once_control;
};

enum HeadersState//For analysis of the Headers
{
    h_start = 0,
    h_key,
    h_colon,
    h_spaces_after_colon,
    h_value,
    h_CR,
    h_LF,
    h_end_CR,
    h_end_LF
};

class TimerNode;

class RequestData : public std::enable_shared_from_this<RequestData>
{
private:
    int againTimes;
    std::string path;//the path for the request
    int fd; 
    int epollfd;
    std::string content;
    int method;//GET OR POST
    int HTTPversion; //HTTP 1.0 or 1.1
    std::string file_name; //Gets from the HTTP header, first line
    int now_read_pos;//Pointers for the HTTP content
    int state;
    int h_state;
    bool isfinish;
    bool keep_alive;//keep_alive or close
    std::unordered_map<std::string, std::string> headers;
    std::weak_ptr<TimerNode> timer;

private:
    int parse_URI();//Analyse the URI
    int parse_Headers();//Analyse the header for HTTP Version or ....
    int analysisRequest();//Analyse the content of HTTP

public:

    RequestData();
    RequestData(int _epollfd, int _fd, std::string _path);
    ~RequestData();
    void linkTimer(std::shared_ptr<TimerNode> mtimer);//After connection, link the timer and the requestData
    void reset();
    void seperateTimer();//Set the Timers expired
    int getFd();
    void setFd(int _fd);
    void handleRequest();//Call the functions parse_URL(), parse_Headers(),analysisRequest()
    void handleError(int fd, int err_num, std::string short_msg);//Respond with 404 ...
};
