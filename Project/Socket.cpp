#include "Socket.h"

//创建一个非阻塞的SOCKET
int createnonblocking()
{
    int listenfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
    if (listenfd < 0)
    {
        //perror("socket() failed"); exit(-1);
        printf("%s.%s.%d listen socket creat error:%d\n", __FILE__, __FUNCTION__, __LINE__, errno); exit(-1); //在实际开发中一般使用这一行来显示错误
    }
    return listenfd;
}
//构造函数
Socket::Socket(int fd):fd_(fd)
{

}
//析构函数
Socket::~Socket()
{
    ::close(fd_);
}
//返回socket值
int Socket::fd() const
{
    return fd_;
}
//socket选项设置
void Socket::setreuseaddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)); 
}
//socket选项设置
void Socket::setreuseport(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));  
}
//socket选项设置
void Socket::setkeepalive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));  
}
//socket选项设置
void Socket::settcpnodelay(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));  
}
//用于服务端的绑定
void Socket::bind(INETAddress& servaddr)
{
    if (::bind(fd_, servaddr.addr(),sizeof(sockaddr)) < 0 )
    {
        perror("bind() failed"); close(fd_); exit(-1);
    }
}
//用于服务端的监听
void Socket::listen(int Waitqueue)
{
    if (::listen(fd_, Waitqueue) != 0 )
    {
        perror("listen() failed"); close(fd_); exit(-1);
    }
}
//用于服务端创建通信的socket(关键，多看一下)
int Socket::accept(INETAddress& clientaddr)
{
    // 如果是listenfd有事件，表示有新的客户端连上来。
    sockaddr_in peeraddr;
    socklen_t len = sizeof(peeraddr);
    //accept4()函数共有4个参数，相比accept()多了一个flags的参数，用户可以通过此参数直接设置套接字的一些属性，如SOCK_NONBLOCK或者是SOCK_CLOEXEC。
    int clientfd = ::accept4(fd_, (sockaddr*)&peeraddr, &len, SOCK_NONBLOCK);

    clientaddr.setaddr(peeraddr);

    return clientfd;
}


