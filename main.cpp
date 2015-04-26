#include <iostream>
#include "ThreadPool.h"

using namespace std;
using namespace simpleton;

void fun()
{
    for(;;);
    // cout << this_thread::get_id() << endl;
}
int main()
{
    ThreadPool pool(2);
    for(int i = 0;i < 11;i++)
        pool.Submit(fun);
    cout << "fuc" << endl;

    return 0;
}