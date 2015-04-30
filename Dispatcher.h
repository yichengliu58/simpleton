//
// Created by lyc-fedora on 15-4-27.
//
//事件分派器

#ifndef SIMPLETON_DISPATCHER_H
#define SIMPLETON_DISPATCHER_H


#include <sys/epoll.h>

namespace simpleton {
    class Dispatcher {
    public:
        //必须指定与之关联的描述符值
        Dispatcher(int fd)
                : _fd(fd), _events(EPOLLIN), _revents(0)
        { }

        ~Dispatcher() = default;

        //获取描述符值
        int GetFd() const {
            return _fd;
        }

        //设置IO复用返回的事件集合
        void SetReturnEvents(unsigned int revents) {
            _revents = revents;
        }

        //获取需要监听的事件
        unsigned int GetEvents() const {
            return _events;
        }

        //重点是这个处理各种返回事件的函数
        //这里会回调用户提供的各种回调函数
        //回调函数逻辑用户自己提供
        void HandleReturnEvents();

    private:
        //指定本分派器相关联的描述符
        int _fd;
        //表示跟这个描述符相关联的需要监听的事件
        unsigned int _events;
        //表示IO复用后得到的发生的事件
        unsigned int _revents;
    };
}

#endif //SIMPLETON_DISPATCHER_H
