#ifndef __INETADDRESS_H__
#define __INETADDRESS_H__

#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>

// socket的地址协议类
class INETAddress
{
private:
    sockaddr_in addr_;
public:
    INETAddress(const std::string &ip, uint16_t port);   //服务端监听的fd
    INETAddress(const sockaddr_in addr);            //服务端通信的fd
    ~INETAddress();

    const char *ip() const; //返回字符串表示的地址，例如: 192.168.10.2
    uint16_t port() const;  //返回整数表示的端口，例如: 5005
    const sockaddr *addr() const; // 返回addr_成员的地址，转换成了sockaddr(两个可以互相转换)
};

#endif

