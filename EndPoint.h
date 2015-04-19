//
// Created by lyc-fedora on 15-4-19.
//
//封装端口号和ipv4地址，形成一个“终端点”
//具有值语义
//内部有sockaddr_in

#ifndef SIMPLETON_ENDPOINT_H
#define SIMPLETON_ENDPOINT_H

#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdexcept>

namespace simpleton
{
class EndPoint
{
public:
    //使用端口号构造，地址使用INADDR_ANY
    EndPoint(uint16_t port);

    //使用地址和端口号构造
    EndPoint(const std::string& ip,uint16_t port);

    //使用sockaddr_in构造
    EndPoint(const sockaddr_in& addr)
    :_addr(addr)
    { }

    //使用默认拷贝构造函数等

    //几个获取当前值用于打印等方法
    std::string GetIp() const;
    uint16_t GetPort() const;
    std::string ToString() const;

    //直接操作内部结构方法
    const sockaddr_in& GetInternalAddr()
    {
        return _addr;
    }
    void SetInternalAddr(const sockaddr_in& addr)
    {
        _addr = addr;
    }

private:
    sockaddr_in _addr;
};
}

#endif //SIMPLETON_ENDPOINT_H
