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
    cout << ptr->ToString() << endl;
}

int main()
{
    try
    {
        EndPoint local(12302);
        Reactor reactor;
        TcpServer server(&reactor,local);
        server.SetNewConnCallback(fun);
        reactor.Start();
    }
    catch(const std::exception& e)
    {
        cout << e.what() << endl;
    }
    return 0;
}