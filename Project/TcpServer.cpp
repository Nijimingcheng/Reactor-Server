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

    //释放
    for (auto &co : connect_)
        delete co.second;
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
    connection->setclosecallback(std::bind(&TcpServer::closeconnection, this, std::placeholders::_1));
    connection->seterrorcallback(std::bind(&TcpServer::errorconnection, this, std::placeholders::_1));
    
    printf("new connection(fd=%d,ip=%s,port=%d) ok.\n",connection->fd(), connection->ip().c_str(), connection->port());          

    connect_[connection->fd()] = connection;            // 把conn存放map容器中。
}
// 关闭客户端的连接，在Connection类中回调此函数。 
void TcpServer::closeconnection(Connection* connect)
{
    printf("client(eventfd=%d) disconnected.\n", connect->fd());
    connect_.erase(connect->fd());                                  //当连接断开的时候需要把connect从map容器中删除
    delete connect;                                                 //从map中删除connect
}
//客户端的连接错误，在Connection类中回调此函数。
void TcpServer::errorconnection(Connection *connect)
{
    printf("client(eventfd=%d) error.\n",connect->fd());            //当出现错误连接而连接断开的时候需要把connect从map容器中删除
    connect_.erase(connect->fd());                                  //从map中删除connect
    delete connect; 
}
