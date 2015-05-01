//
// Created by lyc-fedora on 15-5-1.
//
//一个最终用来使用的服务器类
//负责接受连接并新建TcpConnection
//本对象负责库和使用者之间的接口
//使用者将自定义的回调函数注册至本对象中

#ifndef SIMPLETON_TCPSERVER_H
#define SIMPLETON_TCPSERVER_H


#include "Reactor.h"
#include "Acceptor.h"

namespace simpleton
{
class TcpServer
{
    using NewConnCallback = function<void(Socket&&,const EndPoint&)>;
public:
    TcpServer(Reactor* reactor,const EndPoint& local);
    ~TcpServer();

    void Start();

    //分别用来设置新连接到来和新的可读消息到来
    void SetNewConnCallback(const NewConnCallback& callback)
    {
        _newConnCallback = callback;
    }
    void SetNewMsgCallback();
private:
    //用于提供给Acceptor的新建连接的回调函数
    void newConnCallback(Socket&&,const EndPoint&&);

    //用于保存关联到的反应器对象
    Reactor* _reactor;
    //本Server内部的接受器
    unique_ptr<Acceptor> _acceptor;
    //保存所有TCP连接对象
    //...
    //用户提供对回调函数
    NewConnCallback _newConnCallback;
};
}


#endif //SIMPLETON_TCPSERVER_H
