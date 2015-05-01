//
// Created by lyc-fedora on 15-4-20.
//

#include "Exceptions.h"

std::string simpleton::exceptions::GetErrnoInfo(int num) {
    switch (num) {
        case 0:
            return "无错误";
        case ENOENT:
            return "指定的文件或目录不存在";
        case EPERM:
            return "需要操作权限";
        case EIO:
            return "IO流错误";
        case EBADF:
            return "文件描述符无效";
        case ECHILD:
            return "子进程状态改变";
        case EACCES:
            return "请求被拒绝";
        case EAGAIN:
            return "请求阻塞";
        case EFAULT:
            return "内存地址访问失败";
        case EINVAL:
            return "参数无效";
        case EPIPE:
            return "管道被关闭";
        case ETIME:
            return "定时器到时";
        case EPROTO:
            return "协议错误";
        case ENOTSOCK:
            return "在非阻塞套接字进行操作";
        case ENETUNREACH:
            return "网络不可达";
        case ENETRESET:
            return "网络被重置";
        case ECONNABORTED:
            return "软件引起的网络中断";
        case EMFILE:
            return "描述符值达到上限";
        case ECONNRESET:
            return "连接被对端重置";
        case ESHUTDOWN:
            return "由于调用shutdown无法写入";
        case ETIMEDOUT:
            return "超时";
        case ECONNREFUSED:
            return "连接被拒绝";
        case EHOSTUNREACH:
            return "主机不可达";
        case EALREADY:
            return "操作已经在进行中";
        case EINPROGRESS:
            return "操作正在被处理";
        case EADDRINUSE:
            return "地址正在被使用";
        default:
            return std::to_string(num);
    }
}