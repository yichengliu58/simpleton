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
    char buf[222];
    ::recv(_socket.GetFd(),buf,sizeof(buf),0);
    if(_onNewMessage)
        _onNewMessage(shared_from_this(),string(buf));
}

void TcpConnection::handleWrite()
{
    cout << "write" << endl;
}

void TcpConnection::handleClose()
{
    cout << "close" << endl;
}

void TcpConnection::handleError()
{
    cout << "err" << endl;
}