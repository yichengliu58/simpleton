//
// Created by lyc-fedora on 15-4-27.
//

#include "Multiplexer.h"
#include "Exceptions.h"

using namespace simpleton;

Multiplexer::Multiplexer()
:_epollfd(-1)
{
    if ((_epollfd = ::epoll_create(5)) == -1)
    {
        int saveError = errno;
        throw exceptions::ApiExecError("epoll_create",saveError);
    }
}

Multiplexer::~Multiplexer()
{
    if(_epollfd >= 0)
        ::close(_epollfd);
}

void Multiplexer::AddDispathcer(Dispatcher& dispatcher)
{

}


void Multiplexer::GetAvailDispatchers(int timeout,vector<Dispatcher&>& result)
{
    //_eventList.clear();
    //BUG！监听的事件数目最大值能否为eventList.size()？
    int eventnum = ::epoll_wait(_epollfd,_eventList.data(),_eventList.size(),timeout);

    //处理得到的各种发生的事件
    if(eventnum > 0)
    {
        for(auto& event : _eventList)
        {
            int fd = event.data.fd;
            auto iter = _dispatcherMap.find(fd);
            if(iter != _dispatcherMap.end())
                (iter->second).SetReturnEvents(event.events);
        }
    }
    else if(eventnum == 0)
    {
        //timeout ???
    }
    else
    {
        int saveError = errno;
        throw exceptions::ApiExecError("epoll_wait",saveError);
    }
}