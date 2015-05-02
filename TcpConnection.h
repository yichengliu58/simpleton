//
// Created by lyc-fedora on 15-5-1.
//
//表示一个Tcp连接
//生命期随连接本身产生终止
//本对象本身会受多个对象引用
//需使用智能指针管理

#ifndef SIMPLETON_TCPCONNECTION_H
#define SIMPLETON_TCPCONNECTION_H

#include "Socket.h"
#include "Reactor.h"

using namespace std;

namespace simpleton
{
//定义四种连接状态
enum ConnState{Connecting,Connected,Disconnecting,Disconnected};

class TcpConnection
{
public:
    TcpConnection(const string& name,Reactor* reactor,Socket&& connSock,const EndPoint& local,const EndPoint& peer);
private:
    //表示连接可读事件回调
    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();

    //本连接的标识符名字用于在TcpServer中查找本连接
    string _name;
    //相关的反应器引用
    Reactor* _reactor;
    //当前连接状态
    ConnState _currState;
    //本TCP连接的连接套接字
    Socket _socket;
    //保存本地连接地址
    EndPoint _localAddr;
    //保存对方连接地址
    EndPoint _peerAddr;
    //自己的事件分发器用于本连接的连接套接字的事件分发
    Dispatcher _dispatcher;
};
}
#endif //SIMPLETON_TCPCONNECTION_H
