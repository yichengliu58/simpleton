//
// Created by lyc-fedora on 15-5-1.
//

#include "TcpServer.h"

using namespace simpleton;

TcpServer::TcpServer(Reactor* reactor, const EndPoint& local)
:_reactor(reactor)
{
    try
    {
        //先新建一个acceptor
        _acceptor.reset(new Acceptor(_reactor,local));
        //没有异常则继续完成Acceptor初始化
        _acceptor->SetNewConnCallback(bind(&TcpServer::_newConnCallback,this));
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

void TcpServer::newConnCallback(Socket&& sock, EndPoint const&& peer)
{
    //在这里新建TcpConnection对象并设置相关属性

}