#include "Connection.h"

Connection::Connection(EventLoop *loop, Socket *clientsock):loop_(loop),clientsock_(clientsock)
{
    clientchannel_ = new Channel(loop_, clientsock->fd());                                   
    clientchannel_->setreadcallback(std::bind(&Connection::onmessage, this));
    clientchannel_->setclosecallback(std::bind(&Connection::closecallback, this));
    clientchannel_->seterrorcallback(std::bind(&Connection::errorcallback, this));
    clientchannel_->setwritecallback(std::bind(&Connection::writecallback,this));
    clientchannel_->useet();                                                                       // 客户端连上来的fd采用边缘触发。
    clientchannel_->enablereading();                                                               // 让epoll_wait()监视clientchannel的读事件。
}

Connection::~Connection()
{
    delete clientsock_;                                                                            //这里客户端的sock声明周期与connection对象一样所以可以在connection中释放                               
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
            inputbuffer_.append(buffer, nread);                                                     //把读取的数据追加到接收缓冲区中。
        }
        else if (nread == -1 && errno == EINTR)                                                     //读取数据的时候被信号中断，继续读取。
        {
            continue;
        }
        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))                      //全部的数据已读取完毕。(Reactor网络库的核心接收数据处理中心)。
        {
            while(true)
            {
                std::string message;                                                                //存放接收缓存区的数据

                if (inputbuffer_.readstaticlen(message) == false) break;                            //接收接收缓冲区的数据
                //暂时先原封不动的发出去先用着
                printf("message (eventfd=%d):%s\n",fd(),message.c_str());

                onmessagecallback_(this, message);
            }
            break;
        }
        else if (nread == 0)                                                                        //客户端连接已断开。
        {
            closecallback();                                                                        //调用连接断开的回调函数, Connection类中实现(回调Connection)
            break;
        }
    }
}
//发送数据
void Connection::send(const char *data, size_t size)
{
    outputbuffer_.appendwithhead(data, size);                                                               //把需要发送的数据保存到Connection的发送缓冲区中。
    clientchannel_->enablewriting();                                                                //注册写事件。
}
//处理写事件的回调函数，供Channel回调。
void Connection::writecallback()
{
    int writen = ::send(fd(), outputbuffer_.data(), outputbuffer_.size(), 0);                       //尝试把outputbuffer_中的数据全部发送出去。
    if (writen > 0) outputbuffer_.erase(0, writen);

    if (outputbuffer_.size() == 0)
    {
        clientchannel_->disablewriting();                                                           //如果发送缓冲区中没有数据了，表示数据已发送完成，不再关注写事件。     
        sendcompletecallback_(this);
    }     
}
//TCP连接关闭(断开)的回调函数, 供Channel回调
void Connection::closecallback()
{
    closecallback_(this);                                                                           //回调TcpServer::closeconnection()。
}
//TCP连接错误的回调函数, 供Channel回调
void Connection::errorcallback()
{
    errorcallback_(this);                                                                           //回调TcpServer::errorconnection()。
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
//设置处理报文的回调函数。
void Connection::setonmessagecallback(std::function<void(Connection*, std::string&)> rb)
{
    onmessagecallback_ = rb;                                                                        //回调TcpServer::onmessage()。
}
//发送数据完成后的回调函数。
void Connection::setsendcompletecallback(std::function<void(Connection*)> rb)
{
    sendcompletecallback_ = rb;
}
