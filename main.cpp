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
        plexer.Wait(-1,list);

        /*vector<epoll_event> v;
        v.reserve(10);
        int r = ::epoll_create(5);
        epoll_event event;
        event.data.fd = server.GetFd();
        event.events = EPOLLIN;
        epoll_ctl(r,EPOLL_CTL_ADD,server.GetFd(),&event);
        int rr = ::epoll_wait(r,v.data(),12,-1);
        cout << rr << endl;*/
    }
    catch(const std::exception& e)
    {
        cout << e.what()<< endl;
    }
    return 0;
}