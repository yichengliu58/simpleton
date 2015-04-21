//
// Created by lyc-fedora on 15-4-20.
//
//定义可能会用到的各种自定义异常类
//抛出这些类可以根据名字确定错误信息
//省去了查看errno

#ifndef SIMPLETON_EXCEPTIONS_H
#define SIMPLETON_EXCEPTIONS_H

#include <stdexcept>
#include <exception>

namespace simpleton
{
    namespace exceptions
    {
        //根据errno获取错误信息
        std::string GetErrnoInfo(int num);


        //无效的IP地址
        class InvalidAddr : std::invalid_argument
        {
        public:
            //接受errno值并初始化异常信息
            explicit InvalidAddr(int num)
            :std::invalid_argument("无效IP地址！错误信息：" + exceptions::GetErrnoInfo(num))
            {
            }
        };

        //设置套接字属性失败
        //包括fcntl和setsockopt
        class SetOptError : std::runtime_error
        {
        public:
            explicit SetOptError(const std::string& name,int num)
            :std::runtime_error(name + "属性设置失败！错误信息：" + exceptions::GetErrnoInfo(num))
            { }
        };

        //API调用失败
        class ApiExecError : std::runtime_error
        {
        public:
            explicit ApiExecError(const std::string& name,int num)
            :std::runtime_error(std::string(name + "调用失败！错误信息：" + exceptions::GetErrnoInfo(num)))
            { }
        };

        //对端过早关闭（重置）连接
        class ConnResetError : std::runtime_error
        {
        public:
            explicit ConnResetError(int sockfd,int num)
            :std::runtime_error("Socket：" + std::to_string(sockfd) + "被对端重置！错误信息：" + exceptions::GetErrnoInfo(num))
            { }
        };

        //Socket描述符创建失败
        class NewSockError : std::runtime_error
        {
        public:
            explicit NewSockError(int num)
            :std::runtime_error("Socket描述符创建失败！错误信息：" + exceptions::GetErrnoInfo(num))
            { }
        };
    }
}





#endif //SIMPLETON_EXCEPTIONS_H
