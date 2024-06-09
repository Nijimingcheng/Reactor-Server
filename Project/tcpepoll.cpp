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

int main(int argc,char *argv[])
{
    if (argc != 3)
    {
        printf("usage: ./tcpepoll ip port\n"); 
        printf("example: ./tcpepoll 172.18.0.3 6006\n\n"); 
        return -1;
    }

    // 创建服务端用于监听的listenfd。在创建监听的SOCKET中 "与上(添加)SOCK_NONBLOCK" 即可直接创造出一个非阻塞的SOCKET
    int listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
    if (listenfd < 0)
    {
        perror("socket() failed"); return -1;
    }

    // 设置listenfd的属性，如果对这些属性不熟悉，百度之。 设置SOCKET的属性要在bind之前设置
    int opt = 1;
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,static_cast<socklen_t>(sizeof opt));    // 必须的。 //IO端口复用的，当服务端关闭本次的socket通信时，会进入一个TIME_WAIT的状态，这时候需要等待一段时间才能受理客户端连接请求。
    setsockopt(listenfd,SOL_SOCKET,TCP_NODELAY   ,&opt,static_cast<socklen_t>(sizeof opt));    // 必须的。禁用Nagle算法，允许小包的发送。对于延时敏感型，同时数据传输量比较小的应用，开启。
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEPORT ,&opt,static_cast<socklen_t>(sizeof opt));    // 有用，但是，在Reactor中意义不大。
    setsockopt(listenfd,SOL_SOCKET,SO_KEEPALIVE   ,&opt,static_cast<socklen_t>(sizeof opt));    // 可能有用，但是，建议自己做心跳。

    INETAddress servaddr(argv[1], atoi(argv[2]));

    if (bind(listenfd, servaddr.addr(),sizeof(sockaddr)) < 0 )
    {
        perror("bind() failed"); close(listenfd); return -1;
    }

    if (listen(listenfd,128) != 0 )        // 在高并发的网络服务器中，第二个参数要大一些。
    {
        perror("listen() failed"); close(listenfd); return -1;
    }

    int epollfd=epoll_create(1);        // 创建epoll句柄（红黑树）。

    // 为服务端的listenfd准备读事件。
    epoll_event ev;              // 声明事件的数据结构。
    ev.data.fd=listenfd;                 // 指定事件的自定义数据，会随着epoll_wait()返回的事件一并返回。
    ev.events=EPOLLIN;                // 让epoll监视listenfd的读事件，采用水平触发。

    epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&ev);     // 把需要监视的listenfd和它的事件加入epollfd中。

    epoll_event evs[10];      // 存放epoll_wait()返回事件的数组。

    while (true)        // 事件循环。
    {
        int infds=epoll_wait(epollfd,evs,10,-1);       // 等待监视的fd有事件发生。

        // 返回失败。
        if (infds < 0)
        {
            perror("epoll_wait() failed"); break;
        }

        // 超时。
        if (infds == 0)
        {
            printf("epoll_wait() timeout.\n"); continue;
        }

        // 如果infds>0，表示有事件发生的fd的数量。
        for (int ii=0;ii<infds;ii++)       // 遍历epoll返回的数组evs。
        {
            //如果是客服端连接的fd有事件，一般分为四种情况
            //第一种：对方已关闭，有些系统检测不到，可以使用EPOLLIN，recv()返回0。
            if (evs[ii].events & EPOLLRDHUP)
            {
                printf("client(eventfd=%d) disconnected.)\n",evs[ii].data.fd);
                close(evs[ii].data.fd);            // 关闭客户端的fd。
            }
            //第二种：接收缓冲区中有数据可以读。
            else if (evs[ii].events & (EPOLLIN|EPOLLPRI))
            {                         //普通数据 | 带外数据
                if (evs[ii].data.fd==listenfd)
                {
                    // 如果是listenfd有事件，表示有新的客户端连上来。
                    sockaddr_in peeraddr;
                    socklen_t len = sizeof(peeraddr);
                    //accept4()函数共有4个参数，相比accept()多了一个flags的参数，用户可以通过此参数直接设置套接字的一些属性，如SOCK_NONBLOCK或者是SOCK_CLOEXEC。
                    int clientfd = accept4(listenfd, (sockaddr*)&peeraddr, &len, SOCK_NONBLOCK);

                    INETAddress clientaddr(peeraddr);

                    printf ("accept client(fd=%d,ip=%s,port=%d) ok.\n",clientfd, clientaddr.ip(), clientaddr.port()); //inet_ntoa函数是将二进制的网络ip转换成十分点进制的网络ip

                    // 为新客户端连接准备读事件，并添加到epoll中。
                    ev.data.fd=clientfd;
                    ev.events=EPOLLIN|EPOLLET;           // 设置读事件，边缘触发。
                    epoll_ctl(epollfd,EPOLL_CTL_ADD,clientfd,&ev);
                }
                else
                {
                    char buffer[1024];
                    while (true)             // 由于使用非阻塞IO，一次读取buffer大小数据，直到全部的数据读取完毕。
                    {
                        bzero(&buffer, sizeof(buffer));
                        ssize_t nread = read(evs[ii].data.fd, buffer, sizeof(buffer));    // 这行代码用了read()，也可以用recv()，一样的，不要纠结。
                        printf("nread = %ld\n", nread);
                        if (nread > 0)      // 成功的读取到了数据。
                        {
                            // 把接收到的报文内容原封不动的发回去。
                            printf("recv(eventfd=%d):%s\n", evs[ii].data.fd, buffer);
                            send(evs[ii].data.fd, buffer, strlen(buffer),0);
                        }
                        else if (nread == -1 && errno == EINTR) // 读取数据的时候被信号中断，继续读取。
                        {
                            continue;
                        }
                        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // 全部的数据已读取完毕。
                        {
                            break;
                        }
                        else if (nread == 0)  // 客户端连接已断开。
                        {
                            printf("client(eventfd=%d) disconnected.\n",evs[ii].data.fd);
                            close(evs[ii].data.fd);            // 关闭客户端的fd。
                            break;
                        }
                    }
                }
            }
            //第三种：有数据需要写，暂时没有代码，以后再说。
            else if (evs[ii].events & EPOLLOUT)
            {
            }
            else //第四种：其它事件，都视为错误。
            {
                printf("client(eventfd=%d) error.\n",evs[ii].data.fd);
                close(evs[ii].data.fd);            // 关闭客户端的fd。
            }
        }
    }
    return 0;
}