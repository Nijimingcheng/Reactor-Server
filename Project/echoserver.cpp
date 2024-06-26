#include "EchoServer.h"

int main(int argc,char *argv[])
{
    if (argc != 3)
    {
        printf("usage: ./echoserver ip port\n");
        printf("example: ./echoserver 172.18.0.3 6006\n\n");
        return -1;
    }

    std::cout << "EchoServer Start\n";

    EchoServer echoserver(argv[1], atoi(argv[2]));
    //启动服务程序
    echoserver.Start();
    return 0;
}
