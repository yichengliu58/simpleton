#include <iostream>
#include "ThreadPool.h"

using namespace std;
using namespace simpleton;

void fun()
{
    for(;;);
}
int main()
{
    simpleton::ThreadPool pool(2);
    for(int i = 0;i < 8;i++)
        pool.Submit(fun);
    return 0;
}