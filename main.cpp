#include <iostream>
#include "ThreadPool.h"

using namespace std;
using namespace simpleton;

int main()
{
    simpleton::ThreadPool pool(2);
    cout << "fuck" << endl;
    return 0;
}