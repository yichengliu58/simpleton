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
#include "TcpConnection.h"

namespace simpleton
{
class TcpServer
{
using TcpConnectionPtr = shared_ptr<TcpConnection>;
using NewConnCallback = function<void(Socket&&,const EndPoint&)>;
public:
    //构造函数完成一个服务器程序的初始化过程
    //内部使用Acceptor完成
    TcpServer(Reactor* reactor,const EndPoint& local);
    ~TcpServer();

    //void Start();

    //分别用来设置新连接到来和新的可读消息到来
    void SetNewConnCallback(const NewConnCallback& callback)
    {
        _newConnCallback = callback;
    }
    //void SetNewMsgCallback();
private:
    //用于提供给Acceptor的新建连接的回调函数
    void handleNewConn(Socket&&,const EndPoint&);

    //用于保存关联到的反应器对象
    Reactor* _reactor;
    //本Server内部的接受器
    unique_ptr<Acceptor> _acceptor;
    //使用map保存TcpConnection对象便于删除连接
    map<string,TcpConnectionPtr> _connections;
    //表示连接的可用ID
    unsigned int _connID;
    //表示本地地址结构
    EndPoint _localAddr;
    //用户提供对回调函数
    NewConnCallback _newConnCallback;
};
}


#endif //SIMPLETON_TCPSERVER_H
