#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include <sys/epoll.h>
#include "Epoll.h"

class Epoll;

class Channel
{
private:
    int fd_ = -1;   //channel拥有的fd(SOCKET句柄), Channel和fd是一对一的关系。
    Epoll *ep_ = nullptr;   //Channel对应的红黑树, Channel与Epoll是多对一的关系, 一个Channel只对应一个Epoll。
    bool inepoll_ = false;  //Channel是否已添加到epoll树上, 如果未添加, 调用epoll_ctl()的时候用EPOLL_CTL_ADD, 否则用EPOLL_CTL_MOD。
    uint32_t events_ = 0;   //fd_需要监视的事件。listenfd和clientfd需要监视EPOLLIN(读事件), clientfd还可能需要监视EPOLLOUT(写事件)。
    uint32_t revents_ = 0;  //fd_已经发生的事件。
public:
    Channel(Epoll* ep, int fd);         //构造函数
    ~Channel();                         //析构函数

    int fd();                           //返回fd_成员
    void useet();                       //采用边缘触发
    void enablereading();               //让epoll_wait()监视fd_的读事件
    void setinepoll();                  //把inepoll_成员的值设置为true
    void setrevents(uint32_t ev);       //设置revents_成员的值为参数ev
    bool inpoll();                      //返回inepoll_成员
    uint32_t events();                  //返回events_成员
    uint32_t revents();                 //返回revents_成员
};

#endif

