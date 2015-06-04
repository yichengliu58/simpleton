//
// Created by lyc-fedora on 15-4-25.
//

#include "Reactor.h"
#include "Exceptions.h"

using namespace simpleton;

thread_local Reactor* Reactor::_check = nullptr;

Reactor::Reactor()
    :_isReacting(false),
     _threadId(this_thread::get_id()),
    _isTaskRunning(false)
{
    //检测本线程是否存在另一个Reactor
    //如果存在则将本对象销毁退出
    if(Reactor::_check)
        throw runtime_error("创建了重复的Reactor对象！");
    else
        _check = this;
    //尝试构造本对像的Multiplexer对象以及eventfd
    //如果产生异常则需要正确释放之前产生的资源
    try
    {
        //一定要先构造Multiplexor
        _multiplexer.reset(new Multiplexer);
        //没有问题再创建eventfd
        int eventfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        if (eventfd < 0)
            throw exceptions::NewSockError(errno);
        //新建本eventfd的分派器
        _eventDispatcher.reset(new Dispatcher(eventfd));
        //设置关注读事件
        _eventDispatcher->SetReadCallback(bind(&Reactor::handleWakeup,this));
        _eventDispatcher->SetReading();
        //添加分派器到本Reactor
        _multiplexer->UpdateDispathcer(_eventDispatcher.get());
    }
    catch(...)
    {
        _isReacting = false;
        _multiplexer.reset(nullptr);
        _eventDispatcher.reset(nullptr);
        Reactor::_check = nullptr;
        throw;
    }

}

Reactor::~Reactor()
{
    _isReacting = false;
    Reactor::_check = nullptr;
}

void Reactor::RunInternally(const Task& task)
{
    //如果在本Reactor线程内部则同步运行否则入队并唤醒
    if (this_thread::get_id() == _threadId)
        task();
    else
        RunInQueue(task);
}

void Reactor::RunInQueue(const Task& task)
{
    //加锁保护
    {
        unique_lock<mutex> guard(_mtx);
        _pendingTasks.emplace_back(task);
    }
    //唤醒使Reactor在循环最后执行
    if(this_thread::get_id() != _threadId || _isTaskRunning)
        Wakeup();
}

void Reactor::Wakeup()
{
    eventfd_t data = 1;
    ssize_t n = ::write(_eventDispatcher->GetFd(),&data,sizeof(data));
    //BUG!!!错误处理
}

void Reactor::handleWakeup()
{
    eventfd_t res = 1;
    ssize_t n = ::read(_eventDispatcher->GetFd(),&res,sizeof(res));
    //BUG!!!! 错误处理暂时没做（n < sizeof(res)？）
}

void Reactor::dealRemainedTasks()
{
    //先把待处理任务交换到当前栈上vector中
    //防止在本函数运行时阻塞别的调用RunInQueue的线程
    vector<Task> tmp;
    _isTaskRunning = true;
    {
        unique_lock<mutex> guard(_mtx);
        tmp.swap(_pendingTasks);
    }
    //依次执行
    for(auto& t : tmp)
        t();
    _isTaskRunning = false;
}

void Reactor::Start()
{
    if(this_thread::get_id() != _threadId)
        throw runtime_error("无法跨线程开启Reactor！");
    _isReacting = true;

    //开始事件循环
    //这里就是整个库的运行原动力！
    while(_isReacting)
    {
        //清除可用分派器列表
        _availDispatchers.clear();
        //开始IO复用
        _multiplexer->Wait(-1,_availDispatchers);
        //得到事件并使用分发器分发
        for(auto& disp : _availDispatchers)
        {
            disp->HandleReturnEvents();
        }
        //处理待运行的任务
        dealRemainedTasks();
    }
}

void Reactor::Stop()
{
    _isReacting = false;
    if(this_thread::get_id() != _threadId)
        Wakeup();
}

void Reactor::UpdateDispatcher(Dispatcher* disp)
{
    if(disp)
        _multiplexer->UpdateDispathcer(disp);
}

void Reactor::DeleteDispatcher(Dispatcher* disp)
{
    if(disp)
        _multiplexer->DeleteDispatcher(disp);
}