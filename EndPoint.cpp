//
// Created by lyc-fedora on 15-4-19.
//
#include "iostream"
#include "EndPoint.h"

using namespace simpleton;

EndPoint::EndPoint(uint16_t port)
{
    bzero(&_addr,sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = ::htonl(INADDR_ANY);
    _addr.sin_port = ::htons(port);
}

EndPoint::EndPoint(const std::string &ip, uint16_t port)
{
    bzero(&_addr,sizeof(_addr));
    _addr.sin_family = AF_INET;
    if(!::inet_pton(AF_INET,ip.c_str(),&_addr.sin_addr))
        throw std::invalid_argument("IP地址无法解析！");
    _addr.sin_port = ::htons(port);
}

std::string EndPoint::GetIp() const
{
    char buf[32];
    ::bzero(buf,32);
    if(!::inet_ntop(AF_INET,&_addr.sin_addr,buf,INET_ADDRSTRLEN))
        throw std::invalid_argument("IP地址无法解析！");
    else
        return std::string(buf);
}

uint16_t EndPoint::GetPort() const
{
    return ::ntohs(_addr.sin_port);
}

std::string EndPoint::ToString() const
{
    return GetIp() + ":" + std::to_string(GetPort());
}

