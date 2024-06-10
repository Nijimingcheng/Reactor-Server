#include "Epoll.h"

//构造函数
Epoll::Epoll()
{
    if ( (epollfd_ = epoll_create(1)) == -1 )        // 创建epoll句柄（红黑树）。
    {
        printf("epoll_create() failed(%d).\n", errno);
        exit(-1);
    }
}
//析构函数
Epoll::~Epoll()
{
    ::close(epollfd_);
}
//把指定事件加入epoll的监视
void Epoll::addfd(int fd, uint32_t op)
{
    epoll_event ev;                 // 声明事件的数据结构。
    ev.data.fd = fd;                // 指定事件的自定义数据，会随着epoll_wait()返回的事件一并返回。
    ev.events = op;                 // 让epoll监视listenfd的读事件，采用水平触发。

    if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev) == -1)     // 把需要监视的listenfd和它的事件加入epollfd中。
    {
        printf("epoll_ctl() failed(%d).\n", errno);
        exit(-1);
    }
}
//Epoll事件集池
std::vector<epoll_event> Epoll::loop(int timeout)
{
    std::vector<epoll_event> evs;   // 存放epoll_wait()返回事件的数组。

    memset(events_, 0, sizeof events_);

    int infds=epoll_wait(epollfd_, events_, MaxEvents_, timeout);       // 等待监视的fd有事件发生。(将发生的事件储存在events_事件中)

    // 返回失败。
    if (infds < 0)
    {
        perror("epoll_wait() failed");
        exit(-1);
    }
    // 超时。
    if (infds == 0)
    {
        printf("epoll_wait() timeout.\n");
        return evs;
    }

    for (int i = 0; i < infds; i ++)
        evs.push_back(events_[i]);
    
    return evs;
}
