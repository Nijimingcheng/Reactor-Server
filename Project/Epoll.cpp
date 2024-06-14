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
// 把channel添加/更新到红黑树上，channel中有fd，也有需要监视的事件。
void Epoll::updatechannel(Channel *ch)
{
    epoll_event ev;             //声明事件的数据结构
    ev.data.ptr = ch;           //指定channel
    ev.events = ch->events();   //指定事件

    if (ch->inpoll())           //如果channel已经在树上了(就改变EPOLL_CTL_ADD成这个参数)
    {
        if (epoll_ctl(epollfd_, EPOLL_CTL_MOD, ch->fd(), &ev) == -1)
        {
            printf("epoll_ctl() failed(%d).\n", errno);
            exit(-1);
        }
    }
    else                        //如果不在channel树上(就把他加入到监听事件中)
    {
        if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, ch->fd(), &ev) == -1)
        {
            printf("epoll_ctl() failed(%d).\n", errno);
            exit(-1);
        }
        ch->setinepoll();       //把该channel事件inepoll参数设置为true(Channel是否已添加到epoll树上)
    }
}
//Epoll事件集池
std::vector<Channel *> Epoll::loop(int timeout)
{
    std::vector<Channel *> channels;   // 存放epoll_wait()返回事件的数组。

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
        return channels;
    }
    // 如果infds>0，表示有事件发生的fd的数量。
    for (int i = 0; i < infds; i ++)
    {
        Channel *ch = (Channel *)events_[i].data.ptr;                   //取出已经发生事件的channel。
        ch->setrevents(events_[i].events);                              //设置channel的revents_成员
        channels.push_back(ch);                                         //把取出的事件加入的事件组中
    }

    return channels;
}
