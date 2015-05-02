//
// Created by lyc-fedora on 15-5-1.
//

#include "Acceptor.h"

using namespace simpleton;

Acceptor::Acceptor(Reactor* reactor, EndPoint end)
:_reactor(reactor),_socket(),_dispatcher(_socket.GetFd()),_isListening(false)
{
    try {
        //新建socket
        _socket.NewSocket();
        //没有异常再重新设置描述符值
        _dispatcher.SetFd(_socket.GetFd());
        _socket.SetReuseAddr(true);
        //绑定地址
        _socket.BindEndPoint(end);
    }
    catch(...)
    {
        _reactor = nullptr;
        throw;
    }
}

Acceptor::~Acceptor() {
    try {
        _dispatcher.DisableAllEvents();
        _reactor->DeleteDispatcher(&_dispatcher);
    }
    catch(...)
    {
        //如果真的发生异常就吃掉！
    }
}

void Acceptor::Listen()
{
    try {
        //先在分派器中添加可读事件
        _dispatcher.SetReadCallback(bind(&Acceptor::handleRead,this));
        //再向Reactor中注册本分派器
        _reactor->UpdateDispatcher(&_dispatcher);
        //如果没有异常则设置套接字为监听模式
        _socket.Listen();
        _isListening = true;
    }
    catch(...)
    {
        _isListening = false;
        throw;
    }
}

void Acceptor::handleRead()
{
    EndPoint peer(0);
    //此时非阻塞accept
    Socket connSock = _socket.Accept(peer);
    //必须判断是否有效
    if(connSock.IsValid())
    {
        if(_newConnCallback)
            _newConnCallback(std::move(connSock),peer);
    }
    //如果连接无效Socket本身可以直接被析构无需处理
    //并且不传递给回调函数
    //因而不会创建TcpConnection
}
