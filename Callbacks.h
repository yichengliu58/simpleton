//
// Created by lyc-fedora on 15-6-3.
//
//所有回调function的别名定义

#ifndef SIMPLETON_CALLBACKS_H
#define SIMPLETON_CALLBACKS_H

#include <memory>
#include <functional>

using namespace std;

namespace simpleton
{
class TcpConnection;
class Buffer;

//Acceptor:
//接受新连接时调用的回调
using AcceptorCallback = function<void(Socket &&, const EndPoint &)>;

//TcpConnection（用户回调）:
//新连接已创建
using ConnEstablishedCallback = function<void(const shared_ptr<TcpConnection> &)>;
//新消息可读
using NewMsgCallback = function<void(const shared_ptr<TcpConnection> &, Buffer &)>;
//连接被动关闭
using PassiveClosingCallback = function<void(const shared_ptr<TcpConnection> &)>;
//从TcpServer中移除连接指针
using ConnRemovedCallback = function<void(const shared_ptr<TcpConnection> &)>;

}
#endif //SIMPLETON_CALLBACKS_H
