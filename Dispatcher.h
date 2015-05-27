//
// Created by lyc-fedora on 15-4-27.
//
//事件分派器
//凡是使用到分派器的地方需要注意
//感兴趣的事件的设置和回调的设置需要分开！

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
            : _fd(fd),
              _events(0),
              _revents(0),
              _isReading(false),
              _isWriting(false)
    { }

    ~Dispatcher() = default;

    Dispatcher(const Dispatcher&) = delete;
    Dispatcher& operator=(const Dispatcher&) = delete;

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
        //注意这里直接设置可读事件
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
        _isReading = true;
        _events |= EPOLLIN | EPOLLRDNORM | EPOLLPRI | EPOLLHUP;
    }
    void UnsetReading()
    {
        _isReading = false;
        _events &= ~(EPOLLIN | EPOLLRDNORM | EPOLLPRI | EPOLLHUP);
    }
    bool IsReadingSet()
    {
        return _isReading;
    }

    void SetWriting()
    {
        _isWriting = true;
        _events |= EPOLLOUT | EPOLLWRNORM;
    }
    void UnsetWriting()
    {
        _isWriting = false;
        _events &= ~(EPOLLOUT | EPOLLWRNORM);
    }

    bool IsWritingSet()
    {
        return _isWriting;
    }

    //移除所有分派器上感兴趣的事件（_events清零）
    void UnsetAllEvents()
    {
        _isReading = false;
        _isWriting = false;
        _events = 0;
    }
private:
    //指定本分派器相关联的描述符
    int _fd;
    //表示跟这个描述符相关联的需要监听的事件
    unsigned int _events;
    //表示IO复用后得到的发生的事件
    unsigned int _revents;

    //分别表示是否关注了读写事件
    bool _isReading;
    bool _isWriting;

    //四种对应的事件回调方法
    function<void()> _closeCallback;
    function<void()> _readCallback;
    function<void()> _writeCallback;
    function<void()> _exceptCallback;
};
}

#endif //SIMPLETON_DISPATCHER_H
