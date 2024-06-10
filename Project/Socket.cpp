#include "Socket.h"


// class Socket
// {
// private:
//     const int fd_;      //Socket持有的fd，在构造函数中传进来。
// public:
//     Socket(int fd);     //构造函数，传入一个已经准备好的fd。
//     ~Socket();          //在析构函数中，将关闭fd_。

//     int fd() const;     //返回fd_成员
//     void setreuseaddr(bool on);    //设置SO_REUSERADDR选项，true-打开，false-关闭
//     void setreuseport(bool on);    //设置SO_REUSEPORT选项，true-打开，false-关闭
//     void settcpnodelay(bool on);   //设置SO_NODELAY选项，true-打开，false-关闭
//     void setkeepalive(bool on);    //设置SO_KEEPALIVE选项，true-打开，false-关闭
//     void bind(INETAddress& servaddr);   //服务端的socket将调用此函数
//     void listen(int Waitqueue = 128);   //服务端的socket将调用此函数
//     void accpet(INETAddress& clientaddr);   //服务端的socket将调用此函数
// };

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

Socket::Socket(int fd):fd_(fd)
{

}

Socket::~Socket()
{
    ::close(fd_);
}

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

void Socket::bind(INETAddress& servaddr)
{
    if (::bind(fd_, servaddr.addr(),sizeof(sockaddr)) < 0 )
    {
        perror("bind() failed"); close(fd_); exit(-1);
    }
}

void Socket::listen(int Waitqueue)
{
    if (::listen(fd_, Waitqueue) != 0 )
    {
        perror("listen() failed"); close(fd_); exit(-1);
    }
}

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

