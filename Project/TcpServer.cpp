#include "TcpServer.h"

//构造函数，需要服务端的ip地址和端口号
TcpServer::TcpServer(const std::string &ip, const uint16_t port)
{
    acceptor_ = new Acceptor(&loop_, ip, port);
    //在创建Acceptor类中设置回调函数(需要传入一个参数, 故这里是展位符)
    acceptor_->setnewconnectioncb(std::bind(&TcpServer::newconnection, this, std::placeholders::_1));
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
//TcpServer的中使用connection函数
void TcpServer::newconnection(Socket *clientsock)
{
    Connection *connection = new Connection(&loop_, clientsock);              
}
