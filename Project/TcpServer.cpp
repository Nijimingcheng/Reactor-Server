#include "TcpServer.h"

//构造函数，需要服务端的ip地址和端口号
TcpServer::TcpServer(const std::string &ip, const uint16_t port)
{
    acceptor_ = new Acceptor(&loop_, ip, port);
}
//析构函数
TcpServer::~TcpServer()
{
    delete acceptor_;
}
//运行事件循环
void TcpServer::start()
{
    loop_.run();
}
