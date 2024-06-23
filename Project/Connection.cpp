#include "Connection.h"

Connection::Connection(EventLoop *loop, Socket *clientsock):loop_(loop),clientsock_(clientsock)
{
    clientchannel_ = new Channel(loop_, clientsock->fd());                                   
    clientchannel_->setreadcallback(std::bind(&Connection::onmessage, this));
    clientchannel_->setclosecallback(std::bind(&Connection::closecallback, this));
    clientchannel_->seterrorcallback(std::bind(&Connection::errorcallback, this));
    clientchannel_->useet();                                                                    // 客户端连上来的fd采用边缘触发。
    clientchannel_->enablereading();                                                            // 让epoll_wait()监视clientchannel的读事件。
}

Connection::~Connection()
{
    delete clientsock_;                                                                         //这里客户端的sock声明周期与connection对象一样所以可以在connection中释放                               
    delete clientchannel_;
}

int Connection::fd() const
{
    return clientsock_->fd();
}                        
std::string Connection::ip() const
{
    return clientsock_->ip();
}
uint16_t Connection::port() const
{
    return clientsock_->port();
}
//处理对端发送过来的消息。
void Connection::onmessage()
{
    char buffer[1024];
    while (true)                                                                                    //由于使用非阻塞IO，一次读取buffer大小数据，直到全部的数据读取完毕。
    {
        bzero(&buffer, sizeof(buffer));
        ssize_t nread = read(fd(), buffer, sizeof(buffer));                                         //这行代码用了read()，也可以用recv()，一样的，不要纠结。
        if (nread > 0)                                                                              //成功的读取到了数据。
        {
            // 把接收到的报文内容原封不动的发回去。
            // printf("recv(eventfd=%d):%s\n", fd_, buffer);
            // send(fd_, buffer, strlen(buffer),0);
            inputbuffer_.append(buffer, nread);                                                     //把读取的数据追加到接收缓冲区中。
        }
        else if (nread == -1 && errno == EINTR)                                                     //读取数据的时候被信号中断，继续读取。
        {
            continue;
        }
        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))                      //全部的数据已读取完毕。(Reactor网络库的核心接收数据处理中心)。
        {
            printf("recv(eventfd=%d):%s\n", fd(), inputbuffer_.data());

            outputbuffer_ = inputbuffer_;                                                           //运算后的结果已存放在outputbuffer_中。
            inputbuffer_.clear();                                                                   //清空readbuffer_。
            //暂时先原封不动的发出去先用着
            send(fd(), outputbuffer_.data(), outputbuffer_.size(), 0);                              //把发送缓冲区中的数据直接send()出去。
            break;
        }
        else if (nread == 0)  // 客户端连接已断开。
        {
            closecallback(); //调用连接断开的回调函数, Connection类中实现(回调Connection)
            break;
        }
    }
}
//TCP连接关闭(断开)的回调函数, 供Channel回调
void Connection::closecallback()
{
    closecallback_(this);       //回调TcpServer::closeconnection()。
}
//TCP连接错误的回调函数, 供Channel回调
void Connection::errorcallback()
{
    errorcallback_(this);       //回调TcpServer::errorconnection()。
}
//设置关闭fd_的回调函数 
void Connection::setclosecallback(std::function<void(Connection *)> rb)
{
    closecallback_ = rb;
}
//设置fd_发生了错误的回调函数  
void Connection::seterrorcallback(std::function<void(Connection *)> rb)
{
    errorcallback_ = rb;
}
