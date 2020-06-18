#include "util.h"
#include <unistd.h>
#include <signal.h>//sigaction(SIGPIPE,&sa,NULL) 
#include <fcntl.h>//O_NONBLOCK
#include <errno.h> //global variable errno
#include <string.h>


ssize_t readn(int fd,void*buff,ssize_t n)
{
    ssize_t nleft = n;
    ssize_t nread = 0;
    ssize_t readSum = 0;
    char*ptr = (char*)buff;

    while(nleft>0)
    {
        if((nread = read(fd,ptr,nleft))<0)
        {
            if(errno == EINTR) //When reading, system occurs an interuption, does not matter
                nread=0;
            else if(errno == EAGAIN) //Read the fd until the fd can not read further
            {
                return readSum; //Return the total number of read char
            }
            else
            {
                return -1;
            }
        }
        else if (nread == 0) // the other side might close the socket;
        {
            break;
        }

        readSum += nread;
        nleft -= nread;
        ptr += nread;
    }
    return readSum;

}


ssize_t writen(int fd,void*buff,ssize_t n)
{
    ssize_t nleft = n;
    ssize_t nwrite = 0;
    ssize_t writeSum = 0; 
    char*ptr = (char*)buff;

    while(nleft > 0)
    {
        if(nwrite = write(fd,buff,nleft)<=0)
        {
            if(errno == EINTR || errno == EAGAIN)
            {
                nwrite = 0;
            }
            else
            {
                return -1;
            }
        }
        writeSum += nwrite;
        nleft -= nwrite;
        ptr += nwrite;
    }
    return writeSum;
}

void handle_for_sigpipe()
{
    //handle_for_sigpipe deals with the situation the other side closed the socket,
    //if continues to write message, the first time OK, and the other side responds with
    //RST, if continues to write, the system will create the sigpipe signal which leads to
    //the end of program.
    struct sigaction sa;
    memset(&sa,'\0',sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if(sigaction(SIGPIPE,&sa,NULL))
        return;
}

int setSocketNonBlocking(int fd)
{
    //setSocketNonBlocking sets the socket in the nonblocking status
    //F_GETFL returns the file descriptor's status
    int flag = fcntl(fd, F_GETFL,0);
    if(flag == -1) return -1;
    //F_SETFL sets the file descriptor's status
    flag |= O_NONBLOCK;
    if(fcntl(fd,F_SETFL,flag) == -1)
        return -1;
    return 0;
}