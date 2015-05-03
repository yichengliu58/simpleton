#include <iostream>
#include <sstream>
#include "ThreadPool.h"
#include "Socket.h"
#include "Multiplexer.h"
#include "Reactor.h"
#include "Acceptor.h"
#include "TcpServer.h"

using namespace std;
using namespace simpleton;

void fun(const shared_ptr<TcpConnection>& ptr)
{
    cout << ptr->GetPeerAddr().ToString() << " has connected" << endl;
}

void fuck(const shared_ptr<TcpConnection>& conn,const string& msg)
{
    cout << conn->GetPeerAddr().ToString() << " says: " << msg << endl;
}

int main()
{
    try
    {
        EndPoint local(12306);
        Reactor reactor;
        TcpServer server(&reactor,local);
        server.SetNewConnCallback(fun);
        server.SetNewMsgCallback(fuck);
        reactor.Start();
    }
    catch(const std::exception& e)
    {
        cout << e.what() << endl;
    }
    return 0;
}