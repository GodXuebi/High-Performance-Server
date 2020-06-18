#pragma once 
#include <unistd.h> //ssize_t
#include "Log/noncopyable.h"
#include "Log/Logging.h"
#include "Log/MutexLock.h"
ssize_t readn(int fd, void *buff, ssize_t n);
ssize_t writen(int fd, void *buff, ssize_t n);
void handle_for_sigpipe();
int setSocketNonBlocking(int fd);
