#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__

#include <map>
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.h"

class TcpServer
{
private:
    EventLoop *mainloop_;                                                           //主事件循环。
    std::vector<EventLoop *> subloops_;                                             //存放从事件循环的容器。
    Acceptor *acceptor_;                                                            //一个TcpServer只有一个Acceptor对象。
    ThreadPool *threadpool_;                                                        //线程池。
    int threadsz_;                                                                  //线程池的大小，即从事件循环的个数。
    std::map<int, Connection *> connect_;                                           //一个TcpServer有多个Connection对象，存放在map容器中。
    std::function<void(Connection*)> newconnectioncb_;                              //回调EchoServer::HandleNewConnection()。
    std::function<void(Connection*)> closeconnectioncb_;                            //回调EchoServer::HandleClose()。
    std::function<void(Connection*)> errorconnectioncb_;                            //回调EchoServer::HandleError()。
    std::function<void(Connection*, std::string &message)> onmessagecb_;            //回调EchoServer::HandleMessage()。
    std::function<void(Connection*)> sendcompletecb_;                               //回调EchoServer::HandleSendComplete()。
    std::function<void(EventLoop*)> timeoutcb_;                                     //回调EchoServer::HandleTimeOut()。
public:
    TcpServer(const std::string &ip, const uint16_t port, int threadsz = 5);       //构造函数，需要服务端的ip地址和端口号。
    ~TcpServer();                                                                   //析构函数。
    void start();                                                                   //运行事件循环。
    void newconnection(Socket *clientsock);                                         //在TcpServer类中处理客户端新连接请求
    void closeconnection(Connection* connect);                                      //关闭客户端的连接，在Connection类中回调此函数。 
    void errorconnection(Connection *connect);                                      //客户端的连接错误，在Connection类中回调此函数。
    void onmessage(Connection *connect, std::string &message);                      //处理客户端的请求报文，在Connection类中回调此函数。
    void sendcomplete(Connection *connect);                                         //数据发送完成后，在Connection类中回调此函数。
    void epolltimeout(EventLoop *loop);                                             //epoll_wait()超时，在EventLoop类中回调此函数。

    void setnewconnectioncb(std::function<void(Connection*)> rb);
    void setcloseconnectioncb(std::function<void(Connection*)> rb);
    void seterrorconnectioncb(std::function<void(Connection*)> rb);
    void setonmessagecb(std::function<void(Connection*, std::string &message)> rb);
    void setsendcompletecb(std::function<void(Connection*)> rb);
    void settimeoutcb(std::function<void(EventLoop*)> rb);
};

#endif
