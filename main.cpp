#include <iostream>
#include "EndPoint.h"
using namespace std;

int main()
{
    simpleton::EndPoint end("182.157.1.111",12);
    cout << end.ToString() << endl;
    return 0;
}