#include <iostream>
#include "EndPoint.h"
#include "ThreadPool.h"

using namespace std;
using namespace simpleton;
//using namespace simpleton::exceptions;

void fun()
{
    cout << "f" << endl;
}
int main()
{
    simpleton::ThreadPool pool(5);
    pool.Start(8);
    return 0;
}