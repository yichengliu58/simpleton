#include <iostream>
#include "ThreadPool.h"
#include "Socket.h"
#include "Multiplexer.h"

using namespace std;
using namespace simpleton;

int main()
{
    try
    {
        Socket server;
        EndPoint end(12399);
        server.NewSocket();
        Multiplexer plexer;
        Dispatcher dispatcher(server.GetFd());
        plexer.AddDispathcer(dispatcher);
        server.BindEndPoint(end);
        server.Listen();
        vector<Dispatcher *> list;
        plexer.WaitForAvailDispatchers(-1, list);
        for (auto &d : list)
            cout << d->GetEvents() << endl;
    }
    catch(const std::exception& e)
    {
        cout << e.what()<< endl;
    }
    return 0;
}