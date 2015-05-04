//
// Created by lyc-fedora on 15-4-19.
//
//封装socket描述符，析构时调用close，不处理对象生命期，因此需要配合智能指针使用
//实现了移动语义

#ifndef SIMPLETON_SOCKET_H
#define SIMPLETON_SOCKET_H

#include <unistd.h>
#include "EndPoint.h"
#include "Exceptions.h"

namespace simpleton
{
class EndPoint;

class Socket
{
public:
    //默认构造函数产生一个无效的Socket对象
    explicit Socket()
    :_sockfd(-1)
    { }

    explicit  Socket(int sockfd)
            :_sockfd(sockfd)
    {
        //描述符值无效直接抛出异常
        if(sockfd < 0)
            throw exceptions::NewSockError(errno);
    }

    ~Socket()
    {
        //描述符值有效才关闭
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
        other._sockfd = -1;
    }
    Socket& operator=(Socket&& other)
    {
        //处理自引用
        if(this == &other)
            return *this;
        _sockfd = other._sockfd;
        other._sockfd = -1;
        return *this;
    }

    //检测描述符值是否有效
    bool IsValid()
    {
        return _sockfd >= 0;
    }

    //获取描述符值
    int GetFd() const
    {
        return _sockfd;
    }

    //基础APIs

    //为本套接字对象新建一个非阻塞描述符值
    //如果对象本身持有有效描述符值则无操作
    //如果新建失败则抛出异常NewSockError
    //需要客户代码处理异常
    void NewSocket();

    //绑定地址
    void BindEndPoint(const EndPoint& endpoint);
    //将套接字设置为监听模式
    void Listen();
    //关闭写端（shutdown WR）
    void ShutdownWrite();
    //关闭读端（shutdown RD）
    void ShutdownRead();

    //获取套接字当前错误（getsockopt）
    int GetSocketError();

    //接受连接
    //成功时返回新的Socket对象并设置EndPoint对象
    //因为客户端立即重置连接产生的错误忽略并返回-1的Socket对象
    //其他非期待错误抛出异常（[UNP] p362）
    Socket Accept(EndPoint& peer);

    //设置是否启用Nagle算法
    void SetNagle(bool on);
    //设置地址是否复用（so_reuseaddr）
    void SetReuseAddr(bool on);
    //设置保活选项
    void SetKeepAlive(bool on);

private:
    //转换地址结构体方法
    static const struct sockaddr* sockaddr_cast(const struct sockaddr_in*);
    static struct sockaddr* sockaddr_cast(struct sockaddr_in*);

    int _sockfd;
};
}

#endif //SIMPLETON_SOCKET_H
