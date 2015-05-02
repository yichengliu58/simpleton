//
// Created by lyc-fedora on 15-4-27.
//

#include <iostream>
#include "Dispatcher.h"


using namespace std;
using namespace simpleton;

//这里统一处理当前得到的所有事件
void Dispatcher::HandleReturnEvents()
{
    //首先处理关闭触发关闭后不会在触发可读事件了
    //对端关闭连接（shutdown write或者close）
    if(_revents & EPOLLRDHUP)
    {
        if(_closeCallback)
            _closeCallback();
    }

    //数据可读情况
    //包括对端关闭连接（EPOLLRDHUP）
    //和普通数据加优先级数据可读
    if(_revents & (EPOLLIN | EPOLLRDHUP | EPOLLRDNORM | EPOLLHUP | EPOLLPRI))
    {
        if(_readCallback)
            _readCallback();
    }
    //数据可写情况
    //包括优先级数据和普通数据
    if(_revents & (EPOLLOUT | EPOLLWRNORM | EPOLLWRBAND))
    {
        if(_writeCallback)
            _writeCallback();
    }
    //错误情况
    if(_revents & (EPOLLERR))
    {
        if(_exceptCallback)
            _exceptCallback();
    }
}