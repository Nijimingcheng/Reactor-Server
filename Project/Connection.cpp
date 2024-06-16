#include "Connection.h"

Connection::Connection(EventLoop *loop, Socket *clientsock):loop_(loop),clientsock_(clientsock)
{
    clientchannel_ = new Channel(loop_, clientsock->fd());                                   
    clientchannel_->setreadcallback(std::bind(&Channel::onmessage, clientchannel_));
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
