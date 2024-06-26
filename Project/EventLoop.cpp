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
        std::vector<Channel *> channels = ep_->loop(20 * 1000);    // 存放epoll_wait()返回事件, 等待监视的fd有事件发生。

        //如果channels为空，表示超时，回调TcpServer::epolltimeout()。
        if (channels.size() == 0)
        {
            epolltimeoutcallback_(this);
        }
        else
        {
            // 遍历epoll返回的数组evs。查看发生的事件
            for (auto &ch : channels)
                ch->handleevent();
        }
    }
}
// 把channel添加/更新到红黑树上，channel中有fd，也有需要监视的事件。
void EventLoop::updatechannel(Channel *ch)                        
{
    ep_->updatechannel(ch);
}
//设置epoll_wait()超时的回调函数
void EventLoop::setepolltimeoutcallback(std::function<void(EventLoop*)> rb)
{
    epolltimeoutcallback_ = rb;
}
