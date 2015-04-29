//
// Created by lyc-fedora on 15-4-27.
//

#include "Multiplexer.h"
#include "Exceptions.h"

using namespace simpleton;

Multiplexer::Multiplexer()
:_epollfd(-1),_eventList(16) //初始构造时产生16个空event结构体
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

void Multiplexer::AddDispathcer(Dispatcher* dispatcher)
{
    int sockfd = dispatcher->GetFd();
    _dispatcherMap[sockfd] = dispatcher;

    struct epoll_event event;
    event.data.fd = sockfd;
    event.events = dispatcher->GetEvents();
    ::epoll_ctl(_epollfd,EPOLL_CTL_ADD,sockfd,&event);
}


void Multiplexer::Wait(int timeout,vector<Dispatcher*>& result)
{
    int eventnum = ::epoll_wait(_epollfd,_eventList.data(), static_cast<int>(_eventList.size()),timeout);

    //处理得到的各种发生的事件
    if(eventnum > 0)
    {
        //首先获取就绪的事件表并填入分派器列表
        for(int i = 0;i < eventnum;i++)
        {
            int fd = _eventList[i].data.fd;
            auto iter = _dispatcherMap.find(fd);
            //如果没找到，只有上帝知道为啥了
            if (iter == _dispatcherMap.end())
                throw exceptions::InternalLogicError("Multiplexer::Wait");
            else
            {
                (iter->second)->SetReturnEvents(_eventList[i].events);
                result.push_back(iter->second);
            }
        }
        //动态改变内部eventList大小
        if(static_cast<unsigned int>(eventnum) == _eventList.size())
            _eventList.resize(_eventList.size() * 2);
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