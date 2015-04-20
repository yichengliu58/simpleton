#include <iostream>
#include "EndPoint.h"
#include "Socket.h"

using namespace std;

int main()
{
    try {
        simpleton::EndPoint end("127.0.0.1", 65530);
        int sock = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
        //cout << sock << endl;
        simpleton::Socket s(sock);
        s.BindEndPoint(end);
        s.Listen();
        simpleton::EndPoint peer(0);
        s.Accept(peer);
    }
    catch(...)
    {

    }
    return 0;
}