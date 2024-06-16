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

void Channel::handleevent()
{
    //第一种：对方已关闭，有些系统检测不到，可以使用EPOLLIN，recv()返回0。
    if (revents_ & EPOLLRDHUP)
    {
        closecallback_();                //调用连接断开的回调函数, Connection类中实现(回调Connection)
    }
    //第二种：接收缓冲区中有数据可以读。
    else if (revents_ & (EPOLLIN|EPOLLPRI))
    {                         //普通数据 | 带外数据
        readcallback_();
    }
    //第三种：有数据需要写，暂时没有代码，以后再说。
    else if (revents_ & EPOLLOUT)
    {
    }
    else //第四种：其它事件，都视为错误。
    {
        errorcallback_();                //调用错误情况的回调函数, Connection类中实现(回调Connection)                      // 关闭客户端的fd。
    }
}
//处理对端发过来的消息
void Channel::onmessage()
{
    char buffer[1024];
    while (true)             // 由于使用非阻塞IO，一次读取buffer大小数据，直到全部的数据读取完毕。
    {
        bzero(&buffer, sizeof(buffer));
        ssize_t nread = read(fd_, buffer, sizeof(buffer));    // 这行代码用了read()，也可以用recv()，一样的，不要纠结。
        if (nread > 0)      // 成功的读取到了数据。
        {
            // 把接收到的报文内容原封不动的发回去。
            printf("recv(eventfd=%d):%s\n", fd_, buffer);
            send(fd_, buffer, strlen(buffer),0);
        }
        else if (nread == -1 && errno == EINTR) // 读取数据的时候被信号中断，继续读取。
        {
            return;
        }
        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // 全部的数据已读取完毕。
        {
            return;
        }
        else if (nread == 0)  // 客户端连接已断开。
        {
            closecallback_(); //调用连接断开的回调函数, Connection类中实现(回调Connection)
            break;
        }
    }
}
//设置fd_读事件的回调函数
void Channel::setreadcallback(std::function<void()> rb)
{
    readcallback_ = rb;
}

void Channel::setclosecallback(std::function<void()> rb)
{
    closecallback_ = rb;
}

void Channel::seterrorcallback(std::function<void()> rb)
{
    errorcallback_ = rb;
}
