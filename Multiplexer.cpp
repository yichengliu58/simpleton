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


vector<Dispatcher> Multiplexer::Poll(int timeout)
{
    _eventList.clear();
    //BUG！监听的事件数目最大值能否为eventList.size()？
    int eventnum = ::epoll_wait(_epollfd,_eventList.data(),_eventList.size(),timeout);
    //处理得到的各种发生的事件
    if(eventnum > 0)
    {
        //构造一个分派器列表
        vector<Dispatcher> result;
        //预置eventnum个位置
        result.reserve(static_cast<unsigned int>(eventnum));
        for(auto& event : _eventList)
        {

        }
    }
}