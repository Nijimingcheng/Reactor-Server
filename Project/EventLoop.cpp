#include "EventLoop.h"

//在构造函数中创建Epoll对象ep_。
EventLoop::EventLoop():ep_(new Epoll)
{

}
//在析构函数中销毁ep_。
EventLoop::~EventLoop()
{
    delete ep_;
}
//运行事件循环
void EventLoop::run()
{
    while (true)    // 事件循环。
    {
        std::vector<Channel *> channels = ep_->loop();    // 存放epoll_wait()返回事件, 等待监视的fd有事件发生。
        // 遍历epoll返回的数组evs。查看发生的事件
        for (auto &ch : channels)
            ch->handleevent();
    }
}
//返回成员ep_
Epoll *EventLoop::ep()
{
    return ep_;
}
