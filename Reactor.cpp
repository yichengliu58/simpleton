//
// Created by lyc-fedora on 15-4-25.
//

#include "Reactor.h"

using namespace simpleton;

//thread_local Reactor* Reactor::_check = nullptr;

Reactor::Reactor()
//:_isReacting(false),_threadId(this_thread::get_id())
{
    //检测本线程是否存在另一个Reactor
    //如果存在则将本对象销毁退出
    /*if(Reactor::_check)
        throw runtime_error("创建了重复的Reactor对象！");
    else
        _check = this;*/
    //尝试构造本对像的Multiplexer对象
    //如果产生异常则需要正确释放之前产生的资源
    /*try
    {
        _multiplexer.reset(new Multiplexer);
    }
    catch(...)
    {
        _isReacting = false;
        //Reactor::_check = nullptr;
    }*/

}

Reactor::~Reactor()
{
    //_isReacting = false;
    //Reactor::_check = nullptr;
}

/*void Reactor::Start()
{
    *//*if(this_thread::get_id() != _threadId)
        throw runtime_error("无法跨线程开启Reactor！");
    _isReacting = true;
    //清除可用分派器列表
    _availDispatchers.clear();
    //开始IO复用*//*


}*/
