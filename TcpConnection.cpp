//
// Created by lyc-fedora on 15-5-1.
//

#include <iostream>
#include "TcpConnection.h"

using namespace simpleton;

TcpConnection::TcpConnection(const string& name,Reactor* reactor, Socket&& connSock, EndPoint const& local, EndPoint const& peer)
:_name(name),_reactor(reactor),_socket(std::move(connSock)),_localAddr(local),_peerAddr(peer),_dispatcher(_socket.GetFd())
{
    //首先设置本连接的分派器的各种回调
    _dispatcher.SetReadCallback(bind(&TcpConnection::handleRead,this));
    _dispatcher.SetCloseCallback(bind(&TcpConnection::handleClose,this));
    //_dispatcher.SetWriteCallback(bind(&TcpConnection::handleWrite,this));
    _dispatcher.SetExceptCallback(bind(&TcpConnection::handleError,this));

    //向reactor中注册本分派器
    reactor->UpdateDispatcher(&_dispatcher);

    //不需要设置地址重用
    //设置本连接socekt的保活选项
    _socket.SetKeepAlive(true);
}

TcpConnection::~TcpConnection()
{
    _reactor = nullptr;
    _currState = Disconnected;
}

void TcpConnection::ConnectionEstablished()
{
    //设置状态
    _currState = Connected;
    //回调
    if(_onConnEstablished)
        _onConnEstablished(shared_from_this());
}

void TcpConnection::handleRead()
{
    //暂时使用peek查看read的返回值
    char buf[65535];
    int res = ::recv(_socket.GetFd(),buf,sizeof(buf),MSG_PEEK);
    //根据不同的返回值调用不同的处理方法
    if(res > 0)
    {
        ::read(_socket.GetFd(),buf,sizeof(buf));
        _onNewMessage(shared_from_this(),buf);
    }
    else if(res == 0)
    {
        //在处理关闭逻辑之前需要将发送缓冲区的东西发送完
        handleClose();
    }
    else
    {
        handleError();
    }
}

void TcpConnection::handleWrite()
{
    cout << "write" << endl;
}

void TcpConnection::handleClose()
{
    shared_ptr<TcpConnection> tmp = shared_from_this();
    _currState = Disconnecting;
    //首先清除分派器上所有事件
    _dispatcher.UnsetAllEvents();
    //从Reactor上移除本连接的分派器
    _reactor->DeleteDispatcher(&_dispatcher);
    //调用被动关闭连接时的回调（由TcpServer提供）
    _onPassiveClosing(tmp);
}

void TcpConnection::handleError()
{
    int err = _socket.GetSocketError();
}