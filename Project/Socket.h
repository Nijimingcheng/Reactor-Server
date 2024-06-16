#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>  
#include <unistd.h>
#include "INETAddress.h"

int createnonblocking(void);

class Socket
{
private:
    const int fd_;      //Socket持有的fd，在构造函数中传进来。
    std::string ip_;    //如果是listenfd，存放服务端监听的ip，如果是客户端连接的fd，存放对端的ip。
    uint16_t port_;     //如果是listenfd，存放服务端监听的port，如果是客户端连接的fd，存放外部端口。
public:
    Socket(int fd);     //构造函数，传入一个已经准备好的fd。
    ~Socket();          //在析构函数中，将关闭fd_。

    int fd() const;                         //返回fd_成员
    std::string ip() const;                      //返回ip_成员
    uint16_t port() const;                  //返回port_成员
    void setreuseaddr(bool on);             //设置SO_REUSERADDR选项，true-打开，false-关闭
    void setreuseport(bool on);             //设置SO_REUSEPORT选项，true-打开，false-关闭
    void settcpnodelay(bool on);            //设置SO_NODELAY选项，true-打开，false-关闭
    void setkeepalive(bool on);             //设置SO_KEEPALIVE选项，true-打开，false-关闭
    void bind(INETAddress& servaddr);       //服务端的socket将调用此函数
    void listen(int Waitqueue = 128);       //服务端的socket将调用此函数
    int accept(INETAddress& clientaddr);    //服务端的socket将调用此函数
};

#endif
