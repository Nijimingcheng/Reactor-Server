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
    EventLoop *loop_;
    Socket *clientsock_;
    Channel *clientchannel_;
public:
    Connection(EventLoop *loop, Socket *clientsock);
    ~Connection();
};

#endif
