#include <iostream>
#include <sstream>
#include "ThreadPool.h"
#include "Socket.h"
#include "Reactor.h"
#include "TcpServer.h"
#include <future>
#include <algorithm>
#include <signal.h>

using namespace std;
using namespace simpleton;

void onConnected(const shared_ptr<TcpConnection>& ptr)
{
    cout << ptr->GetPeerAddr().ToString() << " has connected" << endl;
}

void onNewMessage(const shared_ptr<TcpConnection>& conn,Buffer& msg)
{
    string tmp = msg.GetAllReadable();
    cout << "收到 " << tmp.length() << " 字节" << endl;
    conn->Send(tmp);
}

void onPassiveClosing(const shared_ptr<TcpConnection>& conn)
{
    cout << conn->ToString() << " is closing " << endl;
}

int main()
{
    try
    {
        //::signal(SIGPIPE,SIG_IGN);
        EndPoint local(12358);
        Reactor reactor;
        TcpServer server(&reactor,local,1);
        server.SetNewConnCallback(onConnected);
        server.SetNewMsgCallback(onNewMessage);
        server.SetPassiveClosingCallback(onPassiveClosing);

        reactor.Start();
    }
    catch(const std::exception& e)
    {
        cout << e.what() << endl;
    }
    return 0;
}