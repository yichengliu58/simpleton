//
// Created by lyc-fedora on 15-5-1.
//
#include "TcpConnection.h"

using namespace simpleton;

TcpConnection::TcpConnection(const string& name,Reactor* reactor, Socket&& connSock, EndPoint const& local, EndPoint const& peer)
    :_name(name),
     _reactor(reactor),
     _socket(std::move(connSock)),
     _localAddr(local),_peerAddr(peer),
     _dispatcher(_socket.GetFd())
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

void TcpConnection::ConnectionRemoved()
{
    _currState = Disconnected;
    _dispatcher.UnsetAllEvents();
    _reactor->DeleteDispatcher(&_dispatcher);
    if(_removeConnCallback)
        _removeConnCallback(shared_from_this());
}

void TcpConnection::Send(string const& s) {
    ////BUG！！！这里线程安全性没有处理！

    //如果连接正在关闭则无法发送直接返回
    if (_currState == Connected)
    {
        _outBuffer.Push(s);
        int res = _outBuffer.WriteIntoKernel(_socket);
        //如果还有残留数据则关注可写事件
        if (res && !_dispatcher.IsWritingSet())
        {
            _dispatcher.SetWriting();
            _reactor->UpdateDispatcher(&_dispatcher);
        }
    }
}

void TcpConnection::ForceClose()
{
    if(_currState == Connected)
        _currState = Disconnecting;
    handleClose();
}

void TcpConnection::Shutdown()
{
    ////BUG！！！线程安全性没有处理！
    if(_currState == Connected)
    {
        //设置状态为正在关闭
        _currState = Disconnecting;
        //如果没有关注写事件并且输出缓冲区没有待读出数据则关闭写端
        if (!_dispatcher.IsWritingSet() && _outBuffer.ReadableSize() <= 0)
            _socket.ShutdownWrite();
    }
    //如果已经开始关闭（必定是被动关闭）则不用处理等待移除连接就行
}


void TcpConnection::handleRead()
{
    //首先将内核缓冲区的数据读入到本缓冲区
    ssize_t res = _inBuffer.ReadFromKernel(_socket);
    //根据不同的返回值调用不同的处理方法
    if(res > 0)
    {
        _onNewMessage(shared_from_this(),_inBuffer);
    }
    else if(res == 0)
    {
        handleClose();
    }
    else
    {
        handleError();
    }
}

//只要写事件被关注就会触发这里
void TcpConnection::handleWrite()
{
    //如果写事件正在被关注且缓冲区有数据待读出并写入内核
    if(_dispatcher.IsWritingSet() && _outBuffer.ReadableSize() > 0)
    {
        //则将缓冲区数据继续写入
        bool res = _outBuffer.WriteIntoKernel(_socket);
        //如果数据写完的话
        if (!res)
        {
            //取消写事件
            _dispatcher.UnsetWriting();
            _reactor->UpdateDispatcher(&_dispatcher);
            //如果正在主动关闭则直接关闭
            //关闭写端后等待对方关闭其写端
            //本端再执行handleClose
            if (_currState == Disconnecting)
                _socket.ShutdownWrite();
        }
    }
}


//只要进入本事件处理方法内则说明对方关闭了连接
//不论是关闭写端还是直接关闭（close）
//那么不能在发送任何数据
//缓冲区中未发送的应该被丢弃（直接析构）
void TcpConnection::handleClose()
{
    //异常情况未处理！！！！！
    if (_currState == Connected || _currState == Disconnecting)
    {
        //清除分派器上所有事件
        _dispatcher.UnsetAllEvents();
        //用户回调
        if (_onPassiveClosing)
            _onPassiveClosing(shared_from_this());
        //移除连接
        ConnectionRemoved();
    }
}

void TcpConnection::handleError()
{
    //这里的错误码没有使用！！
    int err = _socket.GetSocketError();
}