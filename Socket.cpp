//
// Created by lyc-fedora on 15-4-19.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/tcp.h>
#include <fcntl.h>
#include "Socket.h"

using namespace simpleton;

const struct sockaddr* Socket::sockaddr_cast(struct sockaddr_in const * addr)
{
    return static_cast<const sockaddr*>(static_cast<const void*>(addr));
}

struct sockaddr* Socket::sockaddr_cast(struct sockaddr_in* addr)
{
    return static_cast<sockaddr*>(static_cast<void*>(addr));
}



void Socket::SetKeepAlive(bool on)
{
    int val = on ? 1 : 0;
    if(::setsockopt(_sockfd,SOL_SOCKET,SO_KEEPALIVE,&val,
                     static_cast<socklen_t >(sizeof(val))) < 0)
        throw std::runtime_error(std::to_string(_sockfd) + "：设置套接字属性失败！");
}

void Socket::SetNagle(bool on)
{
    int val = on ? 1 : 0;
    if(!::setsockopt(_sockfd,IPPROTO_TCP,TCP_NODELAY,&val,
                     static_cast<socklen_t>(sizeof(val))) < 0)
        throw std::runtime_error(std::to_string(_sockfd) + "：设置套接字属性失败！");
}

void Socket::SetReuseAddr(bool on)
{
    int val = on ? 1 : 0;
    if(!::setsockopt(_sockfd,SO_KEEPALIVE,SO_REUSEADDR,&val,
                     static_cast<socklen_t>(sizeof(val))) < 0)
        throw std::runtime_error(std::to_string(_sockfd) +"：设置套接字属性失败！");
}

void Socket::BindEndPoint(const EndPoint& endpoint)
{
    if(::bind(_sockfd,Socket::sockaddr_cast(&endpoint.GetInternalAddr()),
           static_cast<socklen_t>(sizeof(endpoint.GetInternalAddr()))) < 0)
        throw std::runtime_error(std::to_string(errno) + "：绑定套接字地址失败！");
}

void Socket::Listen()
{
    if(::listen(_sockfd,SOMAXCONN) < 0)
        throw std::runtime_error(std::to_string(_sockfd) + "：设置套接字为监听模式失败！");
}

Socket Socket::Accept(EndPoint& peer)
{
    struct sockaddr_in addr;
    socklen_t len = static_cast<socklen_t >(sizeof(addr));
    bzero(&addr,len);
    int connfd = ::accept(_sockfd,Socket::sockaddr_cast(&addr),&len);

    //设置为非阻塞
    int flags = ::fcntl(_sockfd,F_GETFL,0);
    flags |= O_NONBLOCK;
    flags |= O_CLOEXEC;
    int res = ::fcntl(_sockfd,F_SETFL,flags);
    //未作错误处理！

    //如果返回的连接套接字小于零就不必要close
    //直接抛出异常
    if(connfd < 0)
    {
        //先拿到errno
        int err = errno;
        switch(err)
        {
            //客户重置连接时产生（RST）
            case EAGAIN:
            case ECONNABORTED:
            case EPROTO:
            //被信号中断时产生
            // 信号默认处理方式一般是终止进程
            case EINTR:
                throw std::runtime_error(std::to_string(_sockfd) + "：客户端重置连接！");
            default:
                throw std::runtime_error(std::to_string(_sockfd) + "：无法接受新连接！");
        }
    }
    else
    {
        //正常返回
        peer.SetInternalAddr(addr);
        return Socket(connfd);
    }

}



void Socket::ShutdownWrite()
{
    if(::shutdown(_sockfd,SHUT_WR) < 0)
        throw std::runtime_error(std::to_string(_sockfd) + "：关闭套接字写端失败！");
}

void Socket::ShutdownRead()
{
    if(::shutdown(_sockfd,SHUT_RD) < 0)
        throw std::runtime_error(std::to_string(_sockfd) + "：关闭套接字读端失败！");
}