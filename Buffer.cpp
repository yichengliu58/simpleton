//
// Created by lyc-fedora on 15-4-30.
//

#include "Buffer.h"
#include <algorithm>

using namespace std;
using namespace simpleton;

string Buffer::GetAllReadable()
{
    return string(_buffer.begin() + _readIndex,_buffer.begin() + _writeIndex);
}

string Buffer::GetUntilCRLF()
{
    //先查找是否存在CRLF
    char crlf[] = {'\r','\n'};
    auto it = search(_buffer.begin() + _readIndex,_buffer.begin() + _writeIndex,crlf,crlf + 2);
    //没找到的话返回空字符串
    if(it == _buffer.begin() + _writeIndex)
        return string();
    else
    {
        string res(_buffer.begin() + _readIndex,it);
        //修改可读索引值
        _readIndex += res.size();
        //返回字符串
        return res;
    }
}

//一定注意是从缓冲区中读取走数据并发送至内核
//所以这里处理可读区域
bool Buffer::WriteIntoKernel(const Socket& sock)
{
    //如果没有数据可读直接返回false
    if (ReadableSize() <= 0)
        return false;
    size_t readable = ReadableSize();
    //调用send写入数据
    ssize_t res = ::send(sock.GetFd(), WriteableIndex(), readable, 0);
    //移动可读区域索引
    _readIndex += res;
    //如果索引相遇则重新置零
    if(_readIndex == _writeIndex)
    {
        _readIndex = 0;
        _writeIndex = 0;
    }
    //如果写入的字节数小于期望数目返回真
    return res < readable;
}

//从本缓冲区将数据发送至内核
//所以这里移动可写索引（增加可读区域大小）
//使用栈内存分散读来解决不知道读多少数据的问题
size_t Buffer::ReadFromKernel(const Socket& sock)
{
    //开辟一个足够大的栈空间
    //默认线程最大栈空间为8M
    char tmp[TmpBufSize];
    size_t writeable = WriteableSize();
    //进行分散读
    iovec io[2];
    io[0].iov_base = _buffer.data() + _writeIndex;
    io[0].iov_len = writeable;
    io[1].iov_base = tmp;
    io[1].iov_len = sizeof(tmp);

    ssize_t res = readv(sock.GetFd(),io,2);
    //如果读出的数量大于缓冲区空间则将多余的添加进去
    if(res > writeable)
    {
        _writeIndex = _buffer.size();
        Push(tmp,res - writeable);
    }
    //如果不足就不用添加但是需修改可写索引
    else if(res <= writeable)
    {
        _writeIndex += res;
    }
    return static_cast<size_t >(res);
}

void Buffer::Push(const char* msg, size_t len)
{
    //全部的可用空间（将总容量capacity计入）
    size_t totalAvail = _buffer.capacity() - _writeIndex;
    //如果后面的不够但是前面的够用就将元素挪动到前面
    if(len > totalAvail && len <= PreRemainedSize())
    {
        size_t readable = ReadableSize();
        //挪动元素填充前部剩余空间
        copy(_buffer.begin() + _readIndex,_buffer.begin() + _writeIndex,_buffer.begin());
        //将待写入的元素放置在最后
        _buffer.insert(_buffer.begin() + readable,msg,msg + len);
        //修改可读可写索引
        _readIndex = 0;
        _writeIndex = _readIndex + readable;
    }
        //否则要么可以直接插入要么就让他自己自动增长吧
    else
    {
        //直接向后面插入
        _buffer.insert(_buffer.begin() + _writeIndex, msg, msg + len);
        //修改可写索引
        _writeIndex = _writeIndex + len;
    }
}

void Buffer::Push(const string& msg)
{
    //全部的可用空间（将总容量capacity计入）
    size_t totalAvail = _buffer.capacity() - _writeIndex;
    //如果后面的不够但是前面的够用就将元素挪动到前面
    if(msg.size() > totalAvail && msg.size() <= PreRemainedSize())
    {
        size_t readable = ReadableSize();
        //挪动元素填充前部剩余空间
        copy(_buffer.begin() + _readIndex,_buffer.begin() + _writeIndex,_buffer.begin());
        //将待写入的元素放置在最后
        _buffer.insert(_buffer.begin() + readable,msg.begin(),msg.end());
        //修改可读可写索引
        _readIndex = 0;
        _writeIndex = _readIndex + readable;
    }
    //否则要么可以直接插入要么就让他自己自动增长吧
    else
    {
        //直接向后面插入
        _buffer.insert(_buffer.begin() + _writeIndex, msg.begin(), msg.end());
        //修改可写索引
        _writeIndex = _writeIndex + msg.size();
    }
}
