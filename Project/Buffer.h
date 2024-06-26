#ifndef __BUFFER_H__
#define __BUFFER_H__

#include<iostream>
#include<string>
#include<cstring>

class Buffer
{
private:
    std::string buf_;                                           //用于存放数据。
public:
    Buffer();
    ~Buffer();

    void append(const char *data, size_t size);                 //把数据追加到buf_中。
    void appendwithhead(const char *data, size_t size);         //把数据追加到buf_中。
    bool readstaticlen(std::string &message);                   //读取接收缓冲区。
    void erase(size_t pos, size_t st);                          //从buf_的pos开始,删除st个字节。
    size_t size();                                              //返回buf_的大小。
    const char *data();                                         //返回buf_的首地址。
    void clear();                                               //清空buf_。
};

#endif
