#include <iostream>
#include "EndPoint.h"
#include "ThreadPool.h"

using namespace std;
using namespace simpleton;
//using namespace simpleton::exceptions;

void fun()
{
    cout << "aaa" << endl;
}
int main()
{
    simpleton::ThreadPool pool(5);
    for(int i = 0;i < 12;i++)
        pool.Submit(fun);
    cout << "fuck" << endl;
    return 0;
}