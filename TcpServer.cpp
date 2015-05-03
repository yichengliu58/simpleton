//
// Created by lyc-fedora on 15-5-1.
//

#include "TcpServer.h"

using namespace std;
using namespace std::placeholders;
using namespace simpleton;

TcpServer::TcpServer(Reactor* reactor, const EndPoint& local)
:_reactor(reactor),_connID(1),_localAddr(local)
{
    try
    {
        //先新建一个acceptor
        _acceptor.reset(new Acceptor(_reactor,local));
        //没有异常则继续完成Acceptor初始化
        //注意这里的bind需要两个占位符
        _acceptor->SetNewConnCallback(bind(&TcpServer::handleNewConn,this,_1,_2));
        _acceptor->Listen();
    }
    catch(...)
    {
        _reactor = nullptr;
        throw;
    }
}

TcpServer::~TcpServer()
{
    _reactor = nullptr;
}

void TcpServer::handleNewConn(Socket&& sock, const EndPoint& peer)
{
    //在这里新建TcpConnection对象并设置相关属性
    //给新连接赋予名字：ID + 地址 + Socket描述符
    string name = to_string(_connID) + _localAddr.ToString() + peer.ToString();
    //创建新连接
    TcpConnectionPtr newConn = make_shared<TcpConnection>(ref(name),_reactor,std::move(sock),ref(_localAddr),ref(peer));
    //将本连接置于映射表中
    _connections[name] = newConn;
    //设置回调
    newConn->SetConnEstablishedCallback(_newConnCallback);
    //用于调用用户回调函数
    newConn->ConnectionEstablished();
}