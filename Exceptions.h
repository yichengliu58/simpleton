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
        std::string GetErrorInfo(int num)
        {
            switch (num)
            {
                case 0: return "无错误";
                case EPERM: return "需要操作权限";
                case EIO: return "IO流错误";
                case EBADF: return "文件描述符无效";
                case ECHILD: return "子进程状态改变";
                case EAGAIN: return "请求阻塞";
                case EINVAL: return "参数无效";
                case EPIPE: return "管道被关闭";
                case ETIME: return "定时器到时";
                case EPROTO: return "协议错误";
                case ENOTSOCK: return "在非阻塞套接字进行操作";
                case ENETUNREACH: return "网络不可达";
                case ENETRESET: return "网络被重置";
                case ECONNABORTED: return "软件引起的网络中断";
                case ECONNRESET: return "连接被对端重置";
                case ESHUTDOWN: return "由于调用shutdown无法写入";
                case ETIMEDOUT: return "超时";
                case ECONNREFUSED: return "连接被拒绝";
                case EHOSTUNREACH: return "主机不可达";
                case EALREADY: return "操作已经在进行中";
                case EINPROGRESS: return "操作正在被处理";
                default: return "未知错误";
            }
        }

        //无效的IP地址
        class InvalidAddr : std::invalid_argument
        {
        public:
            //接受errno值并初始化异常信息
            explicit InvalidAddr(int num)
            :std::invalid_argument("无效IP地址！错误信息：" + exceptions::GetErrorInfo(num))
            {
            }
        };

        //设置套接字属性失败
        //包括fcntl和setsockopt
        class SetOptError : std::runtime_error
        {
        public:
            explicit SetOptError(const std::string& name,int num)
            :std::runtime_error(name + "属性设置失败！错误信息：" + exceptions::GetErrorInfo(num))
            { }
        };

        //API调用失败
        class ApiExecError : std::runtime_error
        {
        public:
            explicit ApiExecError(const std::string& name,int num)
            :std::runtime_error(std::string(name + "调用失败！错误信息：" + exceptions::GetErrorInfo(num)))
            { }
        };

        //对端过早关闭（重置）连接
        class ConnResetError : std::runtime_error
        {
        public:
            explicit ConnResetError(int sockfd,int num)
            :std::runtime_error("Socket：" + std::to_string(sockfd) + "被对端重置！错误信息：" + exceptions::GetErrorInfo(num))
            { }
        };
    }
}







#endif //SIMPLETON_EXCEPTIONS_H
