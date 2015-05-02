//
// Created by lyc-fedora on 15-4-27.
//
//事件分派器

#ifndef SIMPLETON_DISPATCHER_H
#define SIMPLETON_DISPATCHER_H


#include <sys/epoll.h>
#include <functional>

using namespace std;

namespace simpleton
{
class Dispatcher
{
public:
    //必须指定与之关联的描述符值
    Dispatcher(int fd)
            : _fd(fd), _events(0), _revents(0)
    { }

    ~Dispatcher() = default;

    //获取描述符值
    int GetFd() const {
        return _fd;
    }
    //设置描述符值
    void SetFd(int fd)
    {
        _fd = fd;
    }

    //设置IO复用返回的事件集合
    //得到返回的事件后由Reactor负责调用HandleReturnEvents
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

    //分别设置事件回调
    void SetCloseCallback(const function<void()>& close)
    {
        _closeCallback = close;
        _events |= EPOLLRDHUP;
    }
    void SetReadCallback(const function<void()>& read)
    {
        SetReading();
        _readCallback = read;
    }
    void SetWriteCallback(const function<void()>& write)
    {
        _writeCallback = write;
    }
    void SetExceptCallback(const function<void()> except)
    {
        _exceptCallback = except;
        _events |= EPOLLERR;
    }

    //因为可写事件可能会引发busy loop（只要内核写缓冲区可用数据量大于低水位标记即触发）
    //需要分别设置可写事件和取消可写事件
    void SetReading()
    {
        _events |= EPOLLIN | EPOLLRDNORM | EPOLLPRI | EPOLLHUP;
    }
    void UnsetReading()
    {
        _events &= ~(EPOLLIN | EPOLLRDNORM | EPOLLPRI | EPOLLHUP);
    }

    void SetWriting()
    {
        _events |= EPOLLOUT | EPOLLWRNORM;
    }
    void UnsetWriting()
    {
        _events &= ~(EPOLLOUT | EPOLLWRNORM);
    }

    //移除所有分派器上感兴趣的事件（_events清零）
    void DisableAllEvents()
    {
        _events = 0;
    }
private:
    //指定本分派器相关联的描述符
    int _fd;
    //表示跟这个描述符相关联的需要监听的事件
    unsigned int _events;
    //表示IO复用后得到的发生的事件
    unsigned int _revents;

    //四种对应的事件回调方法
    function<void()> _closeCallback;
    function<void()> _readCallback;
    function<void()> _writeCallback;
    function<void()> _exceptCallback;
};
}

#endif //SIMPLETON_DISPATCHER_H
