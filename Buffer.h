//
// Created by lyc-fedora on 15-4-30.
//
//应用层buffer
#ifndef SIMPLETON_BUFFER_H
#define SIMPLETON_BUFFER_H

#include <vector>

using namespace std;

namespace simpleton
{
class Buffer
{
public:
    //定义初始缓冲区大小
    static const unsigned int BufferInitSize = 2048;

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
    size_t Readable()
    {
        return _writeIndex - _readIndex;
    }

    //返回可写区域大小
    //最右边那一块
    size_t Writeable()
    {
        return _buffer.size() - _writeIndex;
    }

    //返回可读区域之前的闲置区大小
    //这个区域表示可读的数据已经被读取剩下的空间可以继续利用
    size_t PreRemained()
    {
        return _readIndex;
    }

    //以下是具体的需要使用的方法



private:
    //内部缓冲区对象
    vector<char> _buffer;

    //内部表示可读和可写的索引
    //总是指向第一个可用的字节处
    //索引还是从零开始
    size_t _readIndex;
    size_t _writeIndex;
};
}
#endif //SIMPLETON_BUFFER_H
