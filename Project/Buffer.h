#ifndef __BUFFER_H__
#define __BUFFER_H__

#include<iostream>
#include<string>

class Buffer
{
private:
    std::string buf_;                                           //用于存放数据。
public:
    Buffer();
    ~Buffer();

    void append(const char *data, size_t size);                 //把数据追加到buf_中。
    size_t size();                                              //返回buf_的大小。
    const char *data();                                         //返回buf_的首地址。
    void clear();                                               //清空buf_。
};

#endif
