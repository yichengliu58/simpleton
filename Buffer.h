//
// Created by lyc-fedora on 15-4-30.
//
//应用层buffer
#ifndef SIMPLETON_BUFFER_H
#define SIMPLETON_BUFFER_H

#include <iostream>
#include <vector>
#include "Socket.h"

using namespace std;

namespace simpleton
{
class Buffer
{
public:
    //定义初始缓冲区大小（以太网MTU）
    static const size_t BufferInitSize = 1536;
    //定义缓冲区读数据时临时栈空间大小（32KB）
    static const size_t TmpBufSize = 1024*32;

    //构造时将两个索引值定位到开头（= 0）
    Buffer()
    :_buffer(BufferInitSize),_readIndex(0),_writeIndex(0)
    { }
    ~Buffer()
    {
        _readIndex = 0;
        _writeIndex = 0;
    }

    //拷贝构造定义为默认
    Buffer(const Buffer&) = default;
    Buffer& operator=(const Buffer&) = default;

    //实现移动语义
    Buffer(Buffer&& other)
    {
        _buffer = std::move(other._buffer);
        _writeIndex = other._writeIndex;
        _readIndex = other._readIndex;
        //将被移动的源对象的索引置零
        other._writeIndex = 0;
        other._readIndex = 0;
    }
    Buffer& operator=(Buffer&& other)
    {
        //处理自引用
        if(this == &other)
            return *this;
        _buffer = std::move(other._buffer);
        _writeIndex = other._writeIndex;
        _readIndex = other._readIndex;
        //将被移动的源对象的索引置零
        other._writeIndex = 0;
        other._readIndex = 0;

        return *this;
    }

    //返回可读区域大小
    //表示中间那一块
    size_t ReadableSize()
    {
        return _writeIndex - _readIndex;
    }

    //返回可写区域大小
    //最右边那一块
    size_t WriteableSize()
    {
        return _buffer.size() - _writeIndex;
    }

    //返回可读区域之前的闲置区大小
    //这个区域表示可读的数据已经被读取剩下的空间可以继续利用
    size_t PreRemainedSize()
    {
        return _readIndex;
    }

    //以下是具体的需要使用的方法

    //读取缓冲区中所有可读数据并以String形式返回
    string GetAllReadable();
    //读取所有可读数据但是不清除缓冲区（不移动索引）
    string PeekAllReadable();
    //读取所有数据直到回车换行符（CRLF）
    //如果所有数据中不存在CRLF则返回空字符串
    bool GetUntilCRLF(string&);



    //当有可读事件获得时调用
    //内部会调用recv从内核缓冲区拿走数据
    //返回值就是recv的返回值
    //可以用于判断是否连接关闭
    ssize_t ReadFromKernel(const Socket&);

    //当缓冲区内部有更多数据待写入时调用
    //将缓冲区内部数据写入内核
    //并判断是否还有残留数据
    //如果还有残留数据返回True否则返回false
    bool WriteIntoKernel(const Socket&);


    //当需要向缓冲区写入数据时使用
    //外部写入接口
    void Push(const string&);
    void Push(const char* ,size_t len);

    //获取指向第一个可写字节处的指针
    char* WriteableIndex()
    {
        return _buffer.data() + _writeIndex;
    }

    //获取第一个指向可读字节处的指针
    const char* ReadableIndex() const
    {
        return _buffer.data() + _readIndex;
    }

private:
    //内部缓冲区对象
    vector<char> _buffer;

    //内部表示可读和可写的索引
    //读写都是相对于缓冲区本身来说的
    //总是指向第一个可用的字节处
    //索引还是从零开始
    size_t _readIndex;
    size_t _writeIndex;
};
}
#endif //SIMPLETON_BUFFER_H
