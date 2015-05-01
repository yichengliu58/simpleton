#include <iostream>
#include "ThreadPool.h"
#include "Socket.h"
#include "Multiplexer.h"
#include "Reactor.h"
#include "Acceptor.h"

using namespace std;
using namespace simpleton;

void callback(Socket&& s,const EndPoint& peer)
{
    cout << peer.ToString() << endl;
}

int main()
{
    try
    {
        EndPoint end("127.0.0.1",19544);
        Reactor reactor;
        Acceptor acceptor(&reactor,end);
        acceptor.SetNewConnCallback(callback);
        acceptor.Listen();

        reactor.Start();
    }
    catch(const std::exception& e)
    {
        cout << e.what() << endl;
    }
    return 0;
}