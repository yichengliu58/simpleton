#include <iostream>
#include "EndPoint.h"
#include "Socket.h"

using namespace std;
//using namespace simpleton;
//using namespace simpleton::exceptions;

int main()
{
    try {
        simpleton::EndPoint end("127.0.0.1", 65530);
        simpleton::Socket s;
        s.NewSocket();
        s.BindEndPoint(end);
        s.Listen();
        simpleton::EndPoint peer(0);
        simpleton::Socket ss = s.Accept(peer);
    }
    catch(...)
    {
        //cout << e.what() << endl;
    }
    return 0;
}