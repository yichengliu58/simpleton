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
        function<void()> fun;
        fun();
    }
    catch(const std::exception& e)
    {
        cout << e.what()<< endl;
    }
    return 0;
}