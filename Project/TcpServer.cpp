#include "TcpServer.h"

//构造函数，需要服务端的ip地址和端口号
TcpServer::TcpServer(const std::string &ip, const uint16_t port)
{
    Socket *serversock = new Socket(createnonblocking());                                        //这里使用的Socket需要使用堆对象，含义就是new出来
    INETAddress servaddr(ip, port);
    serversock->setreuseaddr(true);
    serversock->setreuseport(true);
    serversock->setkeepalive(true);
    serversock->settcpnodelay(true);
    serversock->bind(servaddr);
    serversock->listen();

    Channel *servchannel = new Channel(loop_.ep(), serversock->fd());                                      //把epoll红黑树和服务端的socket传入channel中
    servchannel->setreadcallback(std::bind(&Channel::newconnection, servchannel, serversock));            //指定回调函数
    servchannel->enablereading();                                                                          //让epoll_wait()监视servchannel的读事件
}
//析构函数
TcpServer::~TcpServer()
{

}
//运行事件循环
void TcpServer::start()
{
    loop_.run();
}
