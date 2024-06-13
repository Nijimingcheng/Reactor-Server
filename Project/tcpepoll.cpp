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

    Epoll ep;
    //这里new出来的对象没有释放, 这个问题以后再解决。
    Channel *servchannel = new Channel(&ep, serversock.fd());       //把epoll红黑树和服务端的socket传入channel中
    servchannel->setreadcallback(std::bind(&Channel::newconnection, servchannel, &serversock));          //指定回调函数
    servchannel->enablereading();                                         //让epoll_wait()监视servchannel的读事件

    while (true)    // 事件循环。
    {
        std::vector<Channel *> channels = ep.loop();    // 存放epoll_wait()返回事件, 等待监视的fd有事件发生。
        // 遍历epoll返回的数组evs。查看发生的事件
        for (auto &ch : channels)
            ch->handleevent();
    }
    return 0;
}
