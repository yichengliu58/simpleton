//
// Created by lyc-fedora on 15-5-1.
//

#include "TcpServer.h"

using namespace std;
using namespace std::placeholders;
using namespace simpleton;

TcpServer::TcpServer(Reactor* reactor, const EndPoint& local,unsigned int threadNum)
    :_reactor(reactor),
     _connID(1),
     _localAddr(local),
    _pool()
{
    try
    {
        //先新建一个acceptor
        _acceptor.reset(new Acceptor(_reactor,local));
        //没有异常则继续完成Acceptor初始化
        //注意这里的bind需要两个占位符
        _acceptor->SetNewConnCallback(bind(&TcpServer::handleNewConn,this,_1,_2));
        _acceptor->Listen();
        //如果指定了线程数目则创建线程池
        if(threadNum > 0)
        {
            unsigned int recommend = ThreadPool::RecommendNumber();
            unsigned int queue = 1;
            //如果要求的线程数大于推荐值
            if(threadNum > recommend)
                queue = threadNum - recommend + 1;
            _pool.reset(new ThreadPool(threadNum));
        }
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

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
    //删除连接对象
    _connections.erase(conn->ToString());
}

void TcpServer::handleNewConn(Socket&& sock, const EndPoint& peer)
{
    //在这里新建TcpConnection对象并设置相关属性
    //给新连接赋予名字：ID + 地址 + Socket描述符
    string name = to_string(_connID) + _localAddr.ToString() + peer.ToString();
    //创建新连接
    TcpConnectionPtr newConn = make_shared<TcpConnection>(ref(name),_pool->GetAvailReactor(),std::move(sock),ref(_localAddr),ref(peer));
    //将本连接置于映射表中
    _connections[name] = newConn;
    //设置新建连接的回调
    newConn->SetConnEstablishedCallback(_newConnCallback);
    newConn->SetNewMsgCallback(_newMessageCallback);
    newConn->SetPassiveClosingCallback(_passiveClosingCallback);
    //设置连接被动关闭时调用的本Server的方法来移除该连接
    newConn->SetRemoveThisCallback(bind(&TcpServer::removeConnection,this,_1));
    //调用新建连接后的用户回调
    newConn->ConnectionEstablished();
}