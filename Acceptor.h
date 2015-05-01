//
// Created by lyc-fedora on 15-5-1.
//
//接受器内部使用accept接受新连接
//使用一个Socket对象保存监听socket
//使用一个分派器负责新连接到来事件


#ifndef SIMPLETON_ACCEPTOR_H
#define SIMPLETON_ACCEPTOR_H

#include "Socket.h"
#include "Dispatcher.h"
#include "Reactor.h"

using namespace std;

namespace simpleton
{
class Acceptor
{
using NewConnCallback = function<void(int,const EndPoint&)>;

public:
    //构造函数完成服务端的初始化工作
    //包括新建监听套接字、绑定地址等
    Acceptor(Reactor* reactor,EndPoint end);
    ~Acceptor();

    //分开设置为监听模式
    //因为需要先设置回调再开始监听
    void Listen();

    //设置新连接到来时的回调
    void SetNewConnCallback(NewConnCallback& callback)
    {
        _newConnCallback = callback;
    }

private:
    //用于reactor引用将自己的新连接事件添加进去
    Reactor* _reactor;
    //用于处理连接的监听套接字
    Socket _socket;
    //自己的事件分派器用于向Reactor中注册
    Dispatcher _dispatcher;
    //新连接到来时的回调
    NewConnCallback _newConnCallback;
};
}

#endif //SIMPLETON_ACCEPTOR_H
