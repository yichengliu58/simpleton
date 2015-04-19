//
// Created by lyc-fedora on 15-4-19.
//
//封装socket描述符，析构时调用close，不处理对象生命期，因此需要配合智能指针使用
//实现了移动语义

#ifndef SIMPLETON_SOCKET_H
#define SIMPLETON_SOCKET_H

#include <unistd.h>

namespace simpleton
{
class EndPoint;

class Socket
{
public:
    Socket(int sockfd)
            :_sockfd(sockfd)
    { }
    ~Socket()
    {
        if(_sockfd >= 0)
            ::close(_sockfd);
    }

    //禁止拷贝构造
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    //实现移动语义
    Socket(Socket&& other)
        :_sockfd(other._sockfd)
    {
        other._sockfd = -2;
    }
    Socket& operator=(Socket&& other)
    {
        _sockfd = other._sockfd;
        other._sockfd = -2;
        return *this;
    }

    //基础API

    int GetFd() const
    {
        return _sockfd;
    }

    //绑定地址
    void BindEndPoint(const EndPoint& endpoint);
    //将套接字设置为监听模式
    void Listen();
    //关闭写端（shutdown WR）
    void ShutdownWrite();
    //关闭读端（shutdown RD）
    void ShutdownRead();

    //设置选项方，返回上次的选项值

    //设置是否启用Nagle算法
    bool SetNagle(bool on);
    //设置地址是否复用（so_reuseaddr）
    bool SetReuseAddr(bool on);
    //设置保活选项
    bool SetKeepAlive(bool on);

private:
    int _sockfd;
};
}

#endif //SIMPLETON_SOCKET_H
