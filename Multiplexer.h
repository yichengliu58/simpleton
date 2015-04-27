//
// Created by lyc-fedora on 15-4-27.
//
//多路复用器

#ifndef SIMPLETON_MULTIPLEXER_H
#define SIMPLETON_MULTIPLEXER_H

#include <sys/epoll.h>
#include <unistd.h>
#include <vector>
#include <map>
#include "Dispatcher.h"

using namespace std;

class Reactor;

namespace simpleton
{
class Multiplexer
{
public:
    Multiplexer();
    ~Multiplexer();

    //不可拷贝
    Multiplexer(const Multiplexer&) = delete;
    Multiplexer& operator=(const Multiplexer&) = delete;

    /*//移动语义
    Multiplexer(Multiplexer&& );
    Multiplexer& operator=(Multiplexer&& );*/

    //添加要观察的描述符相应的分派器
    //void AddDispathcer(Dispatcher&);
    //开始运行多路复用函数（轮询）
    //使用移动语义返回发生的事件对应的分派器列表
    //调用方可以使用该分派器列表进行事件分派并处理
    vector<Dispatcher> Poll(int);
private:
    //表示内核事件表描述符（epoll_create）
    int _epollfd;
    //表示每次epoll_wait返回的可用事件列表
    vector<epoll_event> _eventList;
    //表示描述符和事件分发器的映射关系
    map<int,Dispatcher&> _dispatcherMap;
};
}

#endif //SIMPLETON_MULTIPLEXER_H
