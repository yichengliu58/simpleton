//
// Created by lyc-fedora on 15-4-19.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/tcp.h>
#include "Socket.h"
#include "EndPoint.h"

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
    if(!::setsockopt(_sockfd,SOL_SOCKET,SO_KEEPALIVE,&val,
                     static_cast<socklen_t >(sizeof(val))))
        throw std::runtime_error(std::to_string(_sockfd) + "：设置套接字属性失败！");
}

void Socket::SetNagle(bool on)
{
    int val = on ? 1 : 0;
    if(!::setsockopt(_sockfd,IPPROTO_TCP,TCP_NODELAY,&val,
                     static_cast<socklen_t>(sizeof(val))))
        throw std::runtime_error(std::to_string(_sockfd) + "：设置套接字属性失败！");
}

void Socket::SetReuseAddr(bool on)
{
    int val = on ? 1 : 0;
    if(!::setsockopt(_sockfd,SO_KEEPALIVE,SO_REUSEADDR,&val,
                     static_cast<socklen_t>(sizeof(val))))
        throw std::runtime_error(std::to_string(_sockfd) +"：设置套接字属性失败！");
}

void Socket::BindEndPoint(const EndPoint& endpoint)
{
    if(!::bind(_sockfd,Socket::sockaddr_cast(&endpoint.GetInternalAddr()),
           static_cast<socklen_t>(sizeof(endpoint.GetInternalAddr()))))
        throw std::runtime_error(std::to_string(_sockfd) + "：绑定套接字地址失败！");
}

void Socket::Listen()
{
    if(!::listen(_sockfd,SOMAXCONN))
        throw std::runtime_error(std::to_string(_sockfd) + "：设置套接字为监听模式失败！");
}

void Socket::ShutdownWrite()
{
    if(!::shutdown(_sockfd,SHUT_WR))
        throw std::runtime_error(std::to_string(_sockfd) + "：关闭套接字写端失败！");
}

void Socket::ShutdownRead()
{
    if(!::shutdown(_sockfd,SHUT_RD))
        throw std::runtime_error(std::to_string(_sockfd) + "：关闭套接字读端失败！");
}