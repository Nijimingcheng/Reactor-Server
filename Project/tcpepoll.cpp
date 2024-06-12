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
    servchannel->enablereading();                                   //让epoll_wait()监视servchannel的读事件

    while (true)    // 事件循环。
    {
        std::vector<Channel *> channels = ep.loop();    // 存放epoll_wait()返回事件, 等待监视的fd有事件发生。
        // 遍历epoll返回的数组evs。查看发生的事件
        for (auto &ch : channels)       
        {
            //如果是客服端连接的fd有事件，一般分为四种情况
            //第一种：对方已关闭，有些系统检测不到，可以使用EPOLLIN，recv()返回0。
            if (ch->revents() & EPOLLRDHUP)
            {
                printf("client(eventfd=%d) disconnected.)\n", ch->fd());
                close(ch->fd());            // 关闭客户端的fd。
            }
            //第二种：接收缓冲区中有数据可以读。
            else if (ch->revents() & (EPOLLIN|EPOLLPRI))
            {                         //普通数据 | 带外数据
                if (ch == servchannel)
                {
                    INETAddress clientaddr;
                    //注意, clientsock只能new出来,不能在栈上, 否则析构函数会关闭fd。
                    //还有, 这里new出来的对象没有释放, 这个问题以后再解决
                    Socket *clientsock = new Socket(serversock.accept(clientaddr));

                    printf ("accept client(fd=%d,ip=%s,port=%d) ok.\n",clientsock->fd(), clientaddr.ip(), clientaddr.port()); //inet_ntoa函数是将二进制的网络ip转换成十分点进制的网络ip

                    // 为新客户端连接准备读事件，并添加到epoll中。
                    //ep.addfd(clientsock->fd(), EPOLLIN|EPOLLET);    // 客户端连上来的fd采用边缘触发。
                    Channel *clientchannel = new Channel(&ep,clientsock->fd());   // 这里new出来的对象没有释放，这个问题以后再解决。
                    clientchannel->useet();                           // 客户端连上来的fd采用边缘触发。
                    clientchannel->enablereading();                   // 让epoll_wait()监视clientchannel的读事件。
                }
                else
                {
                    char buffer[1024];
                    while (true)             // 由于使用非阻塞IO，一次读取buffer大小数据，直到全部的数据读取完毕。
                    {
                        bzero(&buffer, sizeof(buffer));
                        ssize_t nread = read(ch->fd(), buffer, sizeof(buffer));    // 这行代码用了read()，也可以用recv()，一样的，不要纠结。
                        if (nread > 0)      // 成功的读取到了数据。
                        {
                            // 把接收到的报文内容原封不动的发回去。
                            printf("recv(eventfd=%d):%s\n", ch->fd(), buffer);
                            send(ch->fd(), buffer, strlen(buffer),0);
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
                            printf("client(eventfd=%d) disconnected.\n",ch->fd());
                            close(ch->fd());            // 关闭客户端的fd。
                            break;
                        }
                    }
                }
            }
            //第三种：有数据需要写，暂时没有代码，以后再说。
            else if (ch->revents() & EPOLLOUT)
            {
            }
            else //第四种：其它事件，都视为错误。
            {
                printf("client(eventfd=%d) error.\n",ch->fd());
                close(ch->fd());            // 关闭客户端的fd。
            }
        }
    }
    return 0;
}
