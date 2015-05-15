//
// Created by lyc-fedora on 15-4-30.
//

#include "Buffer.h"
#include <algorithm>

using namespace std;
using namespace simpleton;

//从缓冲区中读取数据
//操作可读区域
string Buffer::GetAllReadable()
{
    //产生结果字符串
    string res(_buffer.begin() + _readIndex,_buffer.begin() + _writeIndex);
    //修改索引值
    _readIndex += res.size();
    //如果索引重合则归零（一定会重合）
    if(_readIndex == _writeIndex)
    {
        _readIndex = 0;
        _writeIndex = 0;
    }
    return res;
}

string Buffer::PeekAllReadable()
{
    return string(_buffer.begin() + _readIndex,_buffer.begin() + _writeIndex);
}

bool Buffer::GetUntilCRLF(string& res)
{
    //先查找是否存在CRLF
    const char crlf[] = {'a','b'};
    auto it = search(_buffer.begin() + _readIndex,_buffer.begin() + _writeIndex,crlf,crlf + 2);
    //没找到的话返回空字符串
    if(it == _buffer.begin() + _writeIndex)
        return false;
    else
    {
        res = string(_buffer.begin() + _readIndex,it);
        //修改可读索引值
        _readIndex += res.size();
        //返回字符串
        return true;
    }
}

//一定注意是从缓冲区中读取走数据并发送至内核
//所以这里处理可读区域
bool Buffer::WriteIntoKernel(const Socket& sock)
{
    if(!sock.IsValid())
        throw exceptions::InternalLogicError("Buffer::WriteIntoKernel");
    //如果没有数据可读直接返回false
    if (ReadableSize() <= 0)
        return false;
    size_t readable = ReadableSize();
    //调用write写入数据
    ssize_t res = ::write(sock.GetFd(), ReadableIndex(), readable);
    //处理错误
    if(res < 0 && errno != EWOULDBLOCK)
    {
        //出现错误不移动索引直接抛异常
        throw exceptions::ApiExecError("Buffer::WriteIntoKernel",errno);
    }
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

//从内核读取数据至缓冲区
//所以这里移动可写索引（增加可读区域大小）
//使用栈内存分散读来解决不知道读多少数据的问题
ssize_t Buffer::ReadFromKernel(const Socket& sock)
{
    if(!sock.IsValid())
        throw exceptions::InternalLogicError("Buffer::ReadFromKernel");
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
    //进行分散读
    ssize_t res = ::readv(sock.GetFd(),io,2);
    if(res < 0)
        return res;
    //如果读出的数量大于缓冲区空间则将多余的添加进去
    else if(res > static_cast<ssize_t>(writeable))
    {
        _writeIndex = _buffer.size();
        Push(tmp,res - writeable);
    }
    //如果不足就不用添加但是需修改可写索引
    else if(res <= writeable)
        _writeIndex += res;

    return res;
}

void Buffer::Push(const char* msg, size_t len)
{
    //后面全部的可用空间（将总容量capacity计入）
    size_t backwardAvail = _buffer.capacity() - _writeIndex;
    //如果后面的不够但是前面的够用就将元素挪动到前面
    if(len > backwardAvail && len <= PreRemainedSize())
    {
        size_t readable = ReadableSize();
        //挪动元素填充前部剩余空间
        copy(_buffer.begin() + _readIndex,_buffer.begin() + _writeIndex,_buffer.begin());
        //将待写入的元素在后面直接赋值覆盖（地方足够大因为挪动了）
        auto iter = _buffer.begin() + readable;
        for(int i = 0;i < len;i++)
        {
            *iter = *(msg + i);
            iter++;
        }
        //修改可读可写索引
        _readIndex = 0;
        _writeIndex = _readIndex + readable;
    }
    //否则要么可以直接插入要么就让他自己自动增长吧
    else
    {
        //先重置一下size使后面默认构造的元素被删除
        _buffer.resize(_writeIndex);
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
        //将待写入的元素在后面直接赋值覆盖（地方足够大因为挪动了）
        auto ptr = _buffer.begin() + readable;
        for(int i = 0;i < msg.length();i++)
        {
            *ptr = msg[i];
            ptr++;
        }
        //修改可读可写索引
        _readIndex = 0;
        _writeIndex = _readIndex + readable;
    }
    //否则要么可以直接插入要么就让他自己自动增长吧
    else
    {
        //重置大小
        _buffer.resize(_writeIndex);
        //直接向后面插入
        _buffer.insert(_buffer.begin() + _writeIndex, msg.begin(), msg.end());
        //修改可写索引
        _writeIndex += msg.size();
    }
}
