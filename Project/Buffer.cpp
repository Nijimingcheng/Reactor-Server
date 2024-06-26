#include "Buffer.h"

Buffer::Buffer()
{

}
Buffer::~Buffer()
{

}
// 把数据追加到buf_中。
void Buffer::append(const char *data, size_t size)
{
    buf_.append(data, size);
}
//把数据追加到buf_中，附带报文头部
void Buffer::appendwithhead(const char *data, size_t size)
{
    buf_.append((char *)&size, 4);                                   //将报文头部信息加入到buf中
    buf_.append(data, size);                                         //将报文body部分加入到buf中
}
//从buf_的pos开始,删除st个字节。
void Buffer::erase(size_t pos, size_t st)
{
    buf_.erase(pos, st);
}
// 返回buf_的大小。
size_t Buffer::size()
{
    return buf_.size();
}
// 返回buf_的首地址。
const char *Buffer::data()
{
    return buf_.data();
}
// 清空buf_。
void Buffer::clear()
{
    buf_.clear();
}
//读取接收缓冲区
bool Buffer::readstaticlen(std::string &message)
{
    int len;
    memcpy(&len, buf_.data(), 4);

    if (buf_.size() < len + 4) return false;

    message.append(buf_.data() + 4, len);
    
    erase(0, len + 4);
    return true;
}
