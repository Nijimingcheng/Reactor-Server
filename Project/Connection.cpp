#include "Connection.h"

Connection::Connection(EventLoop *loop, Socket *clientsock):loop_(loop),clientsock_(clientsock)
{
    clientchannel_ = new Channel(loop_, clientsock->fd());                                     
    clientchannel_->setreadcallback(std::bind(&Channel::onmessage, clientchannel_));
    clientchannel_->useet();                                                                    // 客户端连上来的fd采用边缘触发。
    clientchannel_->enablereading();                                                            // 让epoll_wait()监视clientchannel的读事件。
}

Connection::~Connection()
{
    delete clientsock_;                                                                         //这里客户端的sock声明周期与connection对象一样所以可以在connection中释放                               
    delete clientchannel_;
}
