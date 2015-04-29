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
        plexer.AddDispathcer(&dispatcher);
        server.BindEndPoint(end);
        server.Listen();
        vector<Dispatcher *> list;
        //plexer.AddDispathcer(&dispatcher);
        plexer.Wait(-1, list);
        for (auto &c : list)
            c->HandleReturnEvents();
    }
    catch(const std::exception& e)
    {
        cout << e.what()<< endl;
    }
    return 0;
}