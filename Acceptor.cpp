//
// Created by lyc-fedora on 15-5-1.
//

#include "Acceptor.h"

using namespace simpleton;

Acceptor::Acceptor(Reactor* reactor, EndPoint end)
:_reactor(reactor),_socket(),_dispatcher()
{

}
