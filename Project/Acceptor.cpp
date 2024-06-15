#include "Acceptor.h"

// class Acceptor
// {
// private:
//     EventLoop *loop_;               // Acceptor对应的事件循环，在构造函数中传入。
//     Socket *servsock_;              // 服务端用于监听的socket，在构造函数中创建。
//     Channel *acceptchannel_;        // Acceptor对应的channel，在构造函数中创建。
// public:
//     Acceptor(EventLoop * loop, const std::string &ip, const uint16_t port);
//     ~Acceptor();
// };

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
    acceptchannel_->setreadcallback(std::bind(&Channel::newconnection, acceptchannel_, servsock_));            
    acceptchannel_->enablereading();
}

Acceptor::~Acceptor()
{
    delete servsock_;          //这两个本身就是类自有的所以new出来在析构函数里释放他们                       
    delete acceptchannel_;
}
