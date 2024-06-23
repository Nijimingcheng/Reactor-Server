#include "Acceptor.h"

Acceptor::Acceptor(EventLoop * loop, const std::string &ip, const uint16_t port):loop_(loop)
{
    servsock_ = new Socket(createnonblocking());            
    INETAddress servaddr(ip, port);
    servsock_->setreuseaddr(true);
    servsock_->setreuseport(true);
    servsock_->setkeepalive(true);
    servsock_->settcpnodelay(true);
    servsock_->bind(servaddr);
    servsock_->listen();

    acceptchannel_ = new Channel(loop_, servsock_->fd());                                      
    acceptchannel_->setreadcallback(std::bind(&Acceptor::newconnection, this));            
    acceptchannel_->enablereading();
}

Acceptor::~Acceptor()
{
    delete servsock_;          //这两个本身就是类自有的所以new出来在析构函数里释放他们                       
    delete acceptchannel_;
}

void Acceptor::newconnection()
{
    INETAddress clientaddr;
    //注意, clientsock只能new出来,不能在栈上, 否则析构函数会关闭fd。
    //还有, 这里new出来的对象没有释放, 这个问题以后再解决
    Socket *clientsock = new Socket(servsock_->accept(clientaddr));
    clientsock->setipport(clientaddr.ip(), clientaddr.port());

    newconnectioncb_(clientsock);       // 回调TcpServer::newconnection()
}
//Acceptor的设置回调函数
void Acceptor::setnewconnectioncb(std::function<void(Socket *)> rb)
{
    newconnectioncb_ = rb;
}
