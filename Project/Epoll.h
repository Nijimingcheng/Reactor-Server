#ifndef __EPOLL_H__
#define __EPOLL_H__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>  
#include "Channel.h"

class Channel;

//Epoll类
class Epoll
{
private:
    static const int MaxEvents_ = 100;   
    int epollfd_ = -1;                   //epoll句柄，在构造函数中创建。
    epoll_event events_[MaxEvents_];     //事件集
public:
    Epoll();
    ~Epoll();

    //void addfd(int fd, uint32_t op);    //把fd和它需要监视的事件添加到红黑树上。
    void updatechannel(Channel *ch);      // 把channel添加/更新到红黑树上，channel中有fd，也有需要监视的事件。
    std::vector<Channel *> loop(int timeout = -1);    //运行epoll_wait(), 等待事件的发生, 已发送的事件用vector容器返回
};

#endif
