#ifndef __ECHOSERVER_H__
#define __ECHOSERVER_H__

#include "EventLoop.h"
#include "Connection.h"
#include "TcpServer.h"

class EchoServer
{
private:
    TcpServer tcpserver_;                                                 
public:
    EchoServer(const std::string &ip, const uint16_t port, int threadsz = 5);
    ~EchoServer();

    void Start();                                                                                                  //启动服务
    void HandleNewConnection(Connection *connect);                                                                 //处理新客户端连接请求，在TcpServer类中回调此函数。
    void HandleCloseConnection(Connection *connect);                                                               //关闭客户端的连接，在TcpServer类中回调此函数。 
    void HandleError(Connection *connect);                                                                         //客户端的连接错误，在TcpServer类中回调此函数。
    void HandleMessage(Connection *connect, std::string &message);                                                                       //处理客户端的请求报文，在TcpServer类中回调此函数。
    void HandleSendComplete(Connection *connect);                                                                  //数据发送完成后，在TcpServer类中回调此函数。
    void HandleTimeOut(EventLoop *loop);                                                                           //epoll_wait()超时，在TcpServer类中回调此函数。
};

#endif
