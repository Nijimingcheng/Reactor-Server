#include "INETAddress.h"

INETAddress::INETAddress(const std::string &ip, uint16_t port)
{
    addr_.sin_family = AF_INET;                         // IPv4网络协议的套接字类型。
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());      // 服务端 用于监听的ip地址。inet_addr函数将点十分制的网络ip转换成二进制的网络ip。 转换成C风格的字符串
    addr_.sin_port = htons(port);
}
// 如果是客户端连上来的fd，用这个构造函数。
INETAddress::INETAddress(const sockaddr_in addr):addr_(addr)
{

}

INETAddress::~INETAddress()
{

}
//返回字符串表示的地址，例如: 192.168.10.2
const char *INETAddress::ip() const
{
    return inet_ntoa(addr_.sin_addr);
}
//返回整数表示的端口，例如: 5005
uint16_t INETAddress::port() const
{
    return ntohs(addr_.sin_port);
}
// 返回addr_成员的地址，转换成了sockaddr(两个可以互相转换)
const sockaddr *INETAddress::addr() const
{
    return (sockaddr *)&addr_;
}

