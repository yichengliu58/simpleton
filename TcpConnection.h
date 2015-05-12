//
// Created by lyc-fedora on 15-5-1.
//
//表示一个Tcp连接
//生命期随连接本身产生终止
//本对象本身会受多个对象引用
//需使用智能指针管理
//使用enable_shared_from_this
//允许获取当前对象智能指针

#ifndef SIMPLETON_TCPCONNECTION_H
#define SIMPLETON_TCPCONNECTION_H

#include "Socket.h"
#include "Reactor.h"
#include "Buffer.h"

using namespace std;

namespace simpleton
{
//定义四种连接状态
enum ConnState{Connecting,Connected,Disconnecting,Disconnected};

class TcpConnection : public enable_shared_from_this<TcpConnection> {
public:
    TcpConnection(const string &name, Reactor *reactor, Socket &&connSock, const EndPoint &local, const EndPoint &peer);

    ~TcpConnection();

    TcpConnection(const TcpConnection &) = delete;

    TcpConnection &operator=(const TcpConnection &) = delete;

    //获得连接标识符字符串
    string ToString() const {
        return _name;
    }

    //获得本连接本地地址
    const EndPoint &GetLocalAddr() const {
        return _localAddr;
    }

    //获取连接对端地址
    const EndPoint &GetPeerAddr() const {
        return _peerAddr;
    }

    //获取本连接关联的连接套接字
    const Socket &GetSocket() const {
        return _socket;
    }

    //设置各种用户回调
    //连接建立完成
    void SetConnEstablishedCallback(const function<void(const shared_ptr<TcpConnection> &)> &cb) {
        _onConnEstablished = cb;
    }

    //新可读消息到来
    void SetNewMsgCallback(const function<void(const shared_ptr<TcpConnection> &, Buffer &)> &cb) {
        _onNewMessage = cb;
    }
    //用于设置从TcpServer移除本连接的回调
    void SetRemoveThisCallback(const function<void(const shared_ptr<TcpConnection> &)> &cb)
    {
        _removeConnCallback = cb;
    }
    //用户关闭连接回调
    void SetPassiveClosingCallback(const function<void(const shared_ptr<TcpConnection> &)> &cb)
    {
        _onPassiveClosing = cb;
    }


    //连接完成初始化后由TcpServer调用负责调用用户回调
    void ConnectionEstablished();
    //连接的移除工作
    //负责从TcpServer和Reactor中移除本连接
    void ConnectionRemoved();
    //用于在此连接上无阻塞发送数据
    //外部发送接口
    void Send(const string&);
    //关闭连接的外部接口
    void Shutdown();
    //强制关闭连接
    //正常连接时则丢弃缓冲区未发送数据
    //正在主动关闭时不等待对端关闭直接结束
    void ForceClose();
private:
    //这些回调供dispatcher调用
    //最初的获得的事件会先调用这里的回调
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
    //表示本连接的输出缓冲区（负责向内核写入数据）
    Buffer _outBuffer;
    //表示输入缓冲区（负责从内核读取数据）
    Buffer _inBuffer;

    //这里的回调由TcpServer提供
    //连接正在被动关闭调用Server的方法将本连接从其中移除
    function<void(const shared_ptr<TcpConnection>&)> _removeConnCallback;

    //这里的回调是用户提供的
    //新连接创建（接受）完成
    function<void(const shared_ptr<TcpConnection>&)> _onConnEstablished;
    //新可读消息到来
    function<void(const shared_ptr<TcpConnection>&,Buffer&)> _onNewMessage;
    //用户注册的连接被动关闭时回调
    function<void(const shared_ptr<TcpConnection>&)> _onPassiveClosing;
};
}
#endif //SIMPLETON_TCPCONNECTION_H
