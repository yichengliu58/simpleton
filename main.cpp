#include <iostream>
#include "ThreadPool.h"
#include "Socket.h"
#include "Multiplexer.h"
#include "Reactor.h"

using namespace std;
using namespace simpleton;

void Callback()
{

}

int main()
{
    try
    {
        EndPoint end("127.0.0.1",12955);
        Socket s;
        s.NewSocket();
        s.SetReuseAddr(true);
        s.BindEndPoint(end);
        s.Listen();

        Dispatcher disp(s.GetFd());
        function<void()> func(Callback);
        disp.SetReadCallback(func);

        Reactor reactor;
        reactor.UpdateDispatcher(&disp);
        reactor.Start();

    }
    catch(const std::exception& e)
    {
        cout << e.what() << endl;
    }
    return 0;
}