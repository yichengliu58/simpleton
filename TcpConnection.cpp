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

void TcpConnection::ConnectionRemoved()
{
    //从Reactor上移除本连接的分派器
    _dispatcher.UnsetAllEvents();
    _reactor->DeleteDispatcher(&_dispatcher);
    //调用被动关闭连接时的回调（由TcpServer提供）
    //将本连接指针移除从而导致本连接对象析构
    //完全关闭

    //此时本连接的引用只有这里的临时变量和TcpServer里的记录
    //本方法执行完毕后连接应该被析构
    auto guard = shared_from_this();
    if(_removeConnCallback)
        _removeConnCallback(guard);
}

void TcpConnection::Send(string const& s)
{
    ////BUG！！！这里线程安全性没有处理！

    //如果已经主动关闭则不能写入数据
    if(_currState == Disconnecting)
        return;
    _outBuffer.Push(s);
    int res = _outBuffer.WriteIntoKernel(_socket);
    //如果还有残留数据则关注可写事件
    if(res)
    {
        _dispatcher.SetWriting();
        _reactor->UpdateDispatcher(&_dispatcher);
    }
}

void TcpConnection::Close() {
    ////BUG！！！线程安全性没有处理！

    //这个保护字段保证本方法只被调用一次
    static bool guard = false;
    //如果调用时发现这个已经被调用过了就啥也不干返回
    if(guard)
        return;
    //检测奇怪的异常条件
    if (_currState == Connecting || _currState == Disconnected)
        throw exceptions::InternalLogicError("TcpConnection::Close");
    //设置状态为正在关闭
    _currState = Disconnecting;
    //如果没有关注写事件并且输出缓冲区没有待读出数据则关闭写端
    if(!_dispatcher.IsWritingSet() && _outBuffer.ReadableSize() <= 0)
    {
        _socket.ShutdownWrite();
        //移除连接
        ConnectionRemoved();
    }
    //做完这些操作将保护字段设置为真防止二次调用
    guard = true;
}


void TcpConnection::handleRead()
{
    //首先将内核缓冲区的数据读入到本缓冲区
    size_t res = _inBuffer.ReadFromKernel(_socket);
    //根据不同的返回值调用不同的处理方法
    if(res > 0)
    {
        _onNewMessage(shared_from_this(),_inBuffer);
    }
    else if(res == 0)
    {
        //连接状态处于正常时
        if(_currState == Connected)
            handleClose();
        //如果已经开始关闭过程则应该是主动关闭了本地写端
        //并且对方也关闭了
        else if(_currState == Disconnecting)
        {
            //关闭本地读端
            _socket.ShutdownRead();
            //移除连接
            ConnectionRemoved();
        }
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
            //如果此时正在关闭（不论主动和被动）则数据也写完了，连接应该被关闭
            if (_currState == Disconnecting)
            {
                _socket.ShutdownWrite();
                ConnectionRemoved();
            }
        }
    }
}

void TcpConnection::handleClose()
{
    //设置为正在关闭
    _currState = Disconnecting;
    //关闭本地读端
    _socket.ShutdownRead();
    //清除分派器上所有事件
    _dispatcher.UnsetAllEvents();
    //调用用户回调
    if(_onPassiveClosing)
        _onPassiveClosing(shared_from_this());
}

void TcpConnection::handleError()
{
    int err = _socket.GetSocketError();
}