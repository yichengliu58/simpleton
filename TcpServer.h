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
#include "ThreadPool.h"

namespace simpleton
{
class TcpServer
{
using TcpConnectionPtr = shared_ptr<TcpConnection>;
public:
    //构造函数完成一个服务器程序的初始化过程
    //内部使用Acceptor完成
    //可以指定多线程的线程数目
    TcpServer(Reactor* reactor,const EndPoint& local,unsigned int);
    ~TcpServer();

    //设置新连接建立后的回调
    void SetNewConnCallback(const function<void(const TcpConnectionPtr&)>& cb)
    {
        _newConnCallback = cb;
    }
    //设置新可读消息到来后的回调
    void SetNewMsgCallback(const function<void(const TcpConnectionPtr&,Buffer&)>& cb)
    {
        _newMessageCallback = cb;
    }
    //设置被动关闭时调用的回调函数
    void SetPassiveClosingCallback(const function<void(const TcpConnectionPtr&)>& cb)
    {
        _passiveClosingCallback = cb;
    }
private:
    //用于提供给Acceptor的新建连接的回调函数
    void handleNewConn(Socket&&,const EndPoint&);
    //在TcpConnection处理关闭事件时调用
    //负责将参数指定的连接对象从本Server对象中的映射表删除
    void removeConnection(const TcpConnectionPtr&);

    //用于保存接受器使用的反应器对象指针
    //该对象在外部由用户创建从而控制整个库的运行动力
    Reactor* _reactor;
    //本Server内部的接受器
    unique_ptr<Acceptor> _acceptor;
    //当前本Server中保存的所有连接
    map<string,TcpConnectionPtr> _connections;
    //表示连接的可用ID
    unsigned int _connID;
    //表示本地地址结构
    EndPoint _localAddr;
    //Reactor线程池
    //所有IOReactor都在该池中创建并由该池管理生命期
    ThreadPool _pool;

    //用户提供的新连接建立后的回调函数
    function<void(const TcpConnectionPtr&)> _newConnCallback;
    //用户提供的新消息到达后的回调函数
    function<void(const TcpConnectionPtr&,Buffer& )> _newMessageCallback;
    //用户提供的被动关闭连接后的回调函数
    function<void(const TcpConnectionPtr&)> _passiveClosingCallback;
};
}


#endif //SIMPLETON_TCPSERVER_H
