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

void onConnected(const shared_ptr<TcpConnection>& ptr)
{
    cout << ptr->GetPeerAddr().ToString() << " has connected" << endl;
}

void onNewMessage(const shared_ptr<TcpConnection>& conn,Buffer& msg)
{
    string gete = msg.GetAllReadable();
    cout << conn->GetPeerAddr().ToString() + " says: " + gete << endl;
    conn->Send(gete);
    conn->Shutdown();
}

void onPassiveClosing(const shared_ptr<TcpConnection>& conn)
{
    cout << conn->ToString() << " is closing " << endl;
}

int main()
{
    try
    {
        EndPoint local(12306);
        Reactor reactor;
        TcpServer server(&reactor,local);
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