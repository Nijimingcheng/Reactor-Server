#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>      // TCP_NODELAY需要包含这个头文件。
#include "INETAddress.h"
#include "Socket.h"
#include "Epoll.h"
#include "Channel.h"
#include "EventLoop.h"

int main(int argc,char *argv[])
{
    if (argc != 3)
    {
        printf("usage: ./tcpepoll ip port\n"); 
        printf("example: ./tcpepoll 172.18.0.3 6006\n\n"); 
        return -1;
    }

    Socket serversock(createnonblocking());
    INETAddress servaddr(argv[1], atoi(argv[2]));
    serversock.setreuseaddr(true);
    serversock.setreuseport(true);
    serversock.setkeepalive(true);
    serversock.settcpnodelay(true);
    serversock.bind(servaddr);
    serversock.listen();

    EventLoop Loop;                                                                                      //创建事件处理类
    Channel *servchannel = new Channel(Loop.ep(), serversock.fd());                                      //把epoll红黑树和服务端的socket传入channel中
    servchannel->setreadcallback(std::bind(&Channel::newconnection, servchannel, &serversock));          //指定回调函数
    servchannel->enablereading();                                                                        //让epoll_wait()监视servchannel的读事件

    Loop.run();                                                                                          //事件集运行
    return 0;
}
