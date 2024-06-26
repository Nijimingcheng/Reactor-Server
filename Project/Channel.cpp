#include "Channel.h"

//构造函数
Channel::Channel(EventLoop* loop, int fd):loop_(loop),fd_(fd)
{

}
//析构函数
Channel::~Channel()
{
    // 在析构函数中，不要销毁loop_，也不能关闭fd_，因为这两个东西不属于Channel类，Channel类只是需要它们，使用它们而已。(关键信息看一眼)
}
//返回fd_成员
int Channel::fd()
{
    return fd_;
}
//采用边缘触发
void Channel::useet()
{
    events_ |= EPOLLET;
}
//让epoll_wait()监视fd_的读事件
void Channel::enablereading()
{
    events_ |= EPOLLIN;
    loop_->updatechannel(this);
}
//取消读读事件
void Channel::disablereading()
{
    events_ &= ~EPOLLIN;
    loop_->updatechannel(this);
}
//注册写事件
void Channel::enablewriting()
{
    events_ |= EPOLLOUT;
    loop_->updatechannel(this);
}      
//取消写事件      
void Channel::disablewriting()
{
    events_ &= ~EPOLLOUT;
    loop_->updatechannel(this);
}
//把inepoll_成员的值设置为true
void Channel::setinepoll()
{
    inepoll_ = true;
}
//设置revents_成员的值为参数ev
void Channel::setrevents(uint32_t ev)
{
    revents_ = ev;
}
//返回inepoll_成员
bool Channel::inpoll()
{
    return inepoll_;
}
//返回events_成员
uint32_t Channel::events()
{
    return events_;
}
//返回revents_成员
uint32_t Channel::revents()
{
    return revents_;
}
//事件处理函数，epoll_wait()返回的时候，执行它
void Channel::handleevent()
{
    //第一种：对方已关闭，有些系统检测不到，可以使用EPOLLIN，recv()返回0。
    if (revents_ & EPOLLRDHUP)
        closecallback_();                   //调用连接断开的回调函数, Connection类中实现(回调Connection)
    //第二种：接收缓冲区中有数据可以读。
    else if (revents_ & (EPOLLIN|EPOLLPRI)) //普通数据 | 带外数据
        readcallback_();
    //第三种：有数据需要写。
    else if (revents_ & EPOLLOUT)
        writecallback_();
    //第四种：其它事件，都视为错误。
    else
        errorcallback_();                   //调用错误情况的回调函数, Connection类中实现(回调Connection)
}
//设置fd_读事件的回调函数
void Channel::setreadcallback(std::function<void()> rb)
{
    readcallback_ = rb;
}
//设置关闭fd_的回调函数
void Channel::setclosecallback(std::function<void()> rb)
{
    closecallback_ = rb;
}
//设置fd_发生了错误的回调函数
void Channel::seterrorcallback(std::function<void()> rb)
{
    errorcallback_ = rb;
}
//设置写事件的回调函数
void Channel::setwritecallback(std::function<void()> rb)
{
    writecallback_ = rb;
}