//
// Created by lyc-fedora on 15-4-27.
//

#include "Multiplexer.h"
#include "Exceptions.h"

using namespace simpleton;

Multiplexer::Multiplexer()
:_epollfd(-1),_eventList(6)
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
    int sockfd = dispatcher.GetFd();
    _dispatcherMap[sockfd] = &dispatcher;

    struct epoll_event event;
    event.data.fd = sockfd;
    event.events = dispatcher.GetEvents();
    ::epoll_ctl(_epollfd,EPOLL_CTL_ADD,sockfd,&event);
}


void Multiplexer::Wait(int timeout,vector<Dispatcher*>& result)
{
    //_eventList.clear();
    //BUG！监听的事件数目最大值能否为eventList.size()？
    //应该每次动态改变！！！！
    int eventnum = ::epoll_wait(_epollfd,_eventList.data(), 16,timeout);

    //处理得到的各种发生的事件
    if(eventnum > 0)
    {
        for(auto& event : _eventList)
        {
            int fd = event.data.fd;
            //是否需要判断是否存在？？？
            Dispatcher* dispatcher = _dispatcherMap[fd];
            dispatcher->SetReturnEvents(event.events);
            result.push_back(dispatcher);
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