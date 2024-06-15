#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__

#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Acceptor.h"

class TcpServer
{
private:
    EventLoop loop_;                                                            //一个TcpServer可以有多个事件循环，现在是单线程，暂时只用一个事件循环
    Acceptor *acceptor_;                                                        //一个TcpServer只有一个Acceptor对象
public:
    TcpServer(const std::string &ip, const uint16_t port);                      //构造函数，需要服务端的ip地址和端口号
    ~TcpServer();                                                               //析构函数
    void start();                                                               //运行事件循环
};

#endif
