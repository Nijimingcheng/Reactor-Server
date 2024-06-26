#include "TcpServer.h"

//构造函数，需要服务端的ip地址和端口号
TcpServer::TcpServer(const std::string &ip, const uint16_t port, int threadsz):threadsz_(threadsz)
{
    mainloop_ = new (EventLoop);                                        //创建主循环事件
    mainloop_->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout, this, std::placeholders::_1));

    acceptor_ = new Acceptor(mainloop_, ip, port);
    //在创建Acceptor类中设置回调函数(需要传入一个参数, 故这里是展位符)
    acceptor_->setnewconnectioncb(std::bind(&TcpServer::newconnection, this, std::placeholders::_1));

    threadpool_ = new ThreadPool(threadsz_);

    for (int i = 0; i < threadsz_; i ++)
    {
        subloops_.push_back(new EventLoop);
        subloops_[i]->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout, this, std::placeholders::_1));
        threadpool_->addtask(std::bind(&EventLoop::run, subloops_[i]));
    }
}
//析构函数
TcpServer::~TcpServer()
{
    delete acceptor_;
    delete mainloop_;

    //释放
    for (auto &co : connect_)
        delete co.second;
}
//运行事件循环
void TcpServer::start()
{
    mainloop_->run();
}
//TcpServer的中使用connection函数
void TcpServer::newconnection(Socket *clientsock)
{
    Connection *connection = new Connection(subloops_[clientsock->fd() % threadsz_], clientsock);
    connection->setclosecallback(std::bind(&TcpServer::closeconnection, this, std::placeholders::_1));
    connection->seterrorcallback(std::bind(&TcpServer::errorconnection, this, std::placeholders::_1));
    connection->setonmessagecallback(std::bind(&TcpServer::onmessage, this, std::placeholders::_1, std::placeholders::_2));
    connection->setsendcompletecallback(std::bind(&TcpServer::sendcomplete,this,std::placeholders::_1));

    connect_[connection->fd()] = connection;                               //把conn存放map容器中。
    if(newconnectioncb_) newconnectioncb_(connection);                     //回调EchoServer::HandleNewConnection()。
}
// 关闭客户端的连接，在Connection类中回调此函数。 
void TcpServer::closeconnection(Connection* connect)
{
    if(closeconnectioncb_) closeconnectioncb_(connect);                    //回调EchoServer::HandleClose()。

    connect_.erase(connect->fd());                                         //当连接断开的时候需要把connect从map容器中删除
    delete connect;                                                        //从map中删除connect
}
//客户端的连接错误，在Connection类中回调此函数。
void TcpServer::errorconnection(Connection *connect)
{
    if(errorconnectioncb_) errorconnectioncb_(connect);                    //回调EchoServer::HandleError()。

    connect_.erase(connect->fd());                                         //从map中删除connect
    delete connect; 
}
// 处理客户端的请求报文，在Connection类中回调此函数，在这里对收到的报文进行处理
void TcpServer::onmessage(Connection *connect, std::string &message)
{
    if(onmessagecb_) onmessagecb_(connect, message);                       //回调EchoServer::HandleMessage()。
}
//数据发送完成后，在Connection类中回调此函数。
void TcpServer::sendcomplete(Connection *connect)
{
    if(sendcompletecb_) sendcompletecb_(connect);                          //回调EchoServer::HandleSendComplete()。
}
//epoll_wait()超时，在EventLoop类中回调此函数。
void TcpServer::epolltimeout(EventLoop *loop)
{
    if(timeoutcb_) timeoutcb_(loop);                                       //回调EchoServer::HandleTimeOut()。
}

void TcpServer::setnewconnectioncb(std::function<void(Connection*)> rb)
{
    newconnectioncb_ = rb;
}

void TcpServer::setcloseconnectioncb(std::function<void(Connection*)> rb)
{
    closeconnectioncb_ = rb;
}

void TcpServer::seterrorconnectioncb(std::function<void(Connection*)> rb)
{
    errorconnectioncb_ = rb;
}

void TcpServer::setonmessagecb(std::function<void(Connection*, std::string &message)> rb)
{
    onmessagecb_ = rb;
}

void TcpServer::setsendcompletecb(std::function<void(Connection*)> rb)
{
    sendcompletecb_ = rb;
}

void TcpServer::settimeoutcb(std::function<void(EventLoop*)> rb)
{
    timeoutcb_ = rb;
}
