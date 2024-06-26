#include "EchoServer.h"

EchoServer::EchoServer(const std::string &ip, const uint16_t port):tcpserver_(ip, port)
{
    tcpserver_.setnewconnectioncb(std::bind(&EchoServer::HandleNewConnection, this, std::placeholders::_1));
    tcpserver_.setcloseconnectioncb(std::bind(&EchoServer::HandleCloseConnection, this, std::placeholders::_1));
    tcpserver_.seterrorconnectioncb(std::bind(&EchoServer::HandleError, this, std::placeholders::_1));
    tcpserver_.setonmessagecb(std::bind(&EchoServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2));
    tcpserver_.setsendcompletecb(std::bind(&EchoServer::HandleSendComplete, this, std::placeholders::_1));
    tcpserver_.settimeoutcb(std::bind(&EchoServer::HandleTimeOut, this, std::placeholders::_1));
}

EchoServer::~EchoServer()
{

}
//启动服务
void EchoServer::Start()
{
    tcpserver_.start();
}
//处理新客户端连接请求，在TcpServer类中回调此函数
void EchoServer::HandleNewConnection(Connection *connect)
{
    printf ("New connection(fd=%d,ip=%s,port=%d) ok.\n",connect->fd(),connect->ip().c_str(),connect->port());

    //根据业务的需求，在这里可以增加其他的代码
}
//关闭客户端的连接，在TcpServer类中回调此函数
void EchoServer::HandleCloseConnection(Connection *connect)
{
    printf("EchoServer Connect closed\n");

    //根据业务的需求，在这里可以增加其他的代码 
}
//客户端的连接错误，在TcpServer类中回调此函数
void EchoServer::HandleError(Connection *connect)
{
    printf("EchoServer Connect error\n");

    //根据业务的需求，在这里可以增加其他的代码 
}
//处理客户端的请求报文，在TcpServer类中回调此函数
void EchoServer::HandleMessage(Connection *connect, std::string message)
{
    //在这里将进行若干次步骤运算
    message = "reply" + message;

    int len = message.size();                                        //计算回应报文的大小。
    std::string tmpbuf((char *)&len, 4);                             //把报文头部填充到回应报文中。
    tmpbuf.append(message);                                          //把报文内容填充到回应报文中。

    connect->send(tmpbuf.data(), tmpbuf.size());                     //把临时缓冲区中的数据直接send()出去。
}
//数据发送完成后，在TcpServer类中回调此函数
void EchoServer::HandleSendComplete(Connection *connect)
{
    printf("Message send complete\n");

    //根据业务的需求，在这里可以增加其他的代码     
}
//epoll_wait()超时，在TcpServer类中回调此函数
void EchoServer::HandleTimeOut(EventLoop *loop)
{
    printf("EchoServer timeout\n");

    //根据业务的需求，在这里可以增加其他的代码     
}
