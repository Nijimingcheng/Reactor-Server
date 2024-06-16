#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <functional>
#include "EventLoop.h"
#include "INETAddress.h"
#include "Socket.h"
#include "Channel.h"

class Connection
{
private:
    EventLoop *loop_;                                  //Connection对应的事件循环，在构造函数中传入。 
    Socket *clientsock_;                               //与客户端通讯的Socket。
    Channel *clientchannel_;                           //Connection对应的channel，在构造函数中创建。
    std::function<void(Connection *)> closecallback_;  //关闭fd_的回调函数，将回调TcpServer::closecallback()。
    std::function<void(Connection *)> errorcallback_;  //关闭fd_发生错误的回调函数，将回调TcpServer::errorconnection()。
public:
    Connection(EventLoop *loop, Socket *clientsock);
    ~Connection();

    int fd() const;                         
    std::string ip() const;               
    uint16_t port() const;

    void closecallback();                               //TCP连接关闭(断开)的回调函数, 供Channel回调
    void errorcallback();                               //TCP连接错误的回调函数, 供Channel回调
    void setclosecallback(std::function<void(Connection *)> rb);       //设置关闭fd_的回调函数 
    void seterrorcallback(std::function<void(Connection *)> rb);       //设置fd_发生了错误的回调函数
};  

#endif
