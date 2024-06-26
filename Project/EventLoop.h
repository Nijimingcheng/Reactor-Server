#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include "Epoll.h"
#include <functional>
#include <unistd.h>
#include <sys/syscall.h>

class Epoll;
class Channel;

class EventLoop
{
private:
    Epoll *ep_;                                                                     //每个事件循环只有一个Epoll。
    std::function<void(EventLoop*)> epolltimeoutcallback_;                          //epoll_wait()超时的回调函数。
public:
    EventLoop();                                                                    //在构造函数中创建Epoll对象ep_。
    ~EventLoop();                                                                   //在析构函数中销毁ep_。

    void run();                                                                     //运行事件循环。

    void updatechannel(Channel *ch);                                                //把channel添加/更新到红黑树上，channel中有fd，也有需要监视的事件。
    void setepolltimeoutcallback(std::function<void(EventLoop*)> rb);               //设置epoll_wait()超时的回调函数。
};

#endif
