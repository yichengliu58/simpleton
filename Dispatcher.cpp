//
// Created by lyc-fedora on 15-4-27.
//

#include "Dispatcher.h"

using namespace simpleton;

void Dispatcher::HandleReturnEvents()
{
    //数据可读情况
    //包括对端关闭连接（accept 0）
    //和普通数据加优先级数据可读
    if(_revents & (EPOLLIN | EPOLLRDNORM | EPOLLHUP | EPOLLPRI))
    {
        //
    }
    //数据可写情况
    //包括优先级数据和普通数据
    if(_revents & (EPOLLOUT | EPOLLWRNORM | EPOLLWRBAND))
    {
        //
    }
    //错误情况
    if(_revents & (EPOLLERR))
    {
        //
    }
}