#pragma once 
#include <unistd.h> //ssize_t

ssize_t readn(int fd, void *buff, ssize_t n);
ssize_t writen(int fd, void *buff, ssize_t n);
void handle_for_sigpipe();
int setSocketNonBlocking(int fd);
