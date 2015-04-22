//
// Created by lyc-fedora on 15-4-20.
//
//阻塞队列（生产者消费者队列）
//使用deque实现


#ifndef SIMPLETON_BLOCKINGQUEUE_H
#define SIMPLETON_BLOCKINGQUEUE_H


#include <condition_variable>
#include <memory>
#include <deque>

namespace simpleton
{
    template<typename T>
    class BlockingQueue {
    public:
        //构造时必须指明队列长度
        BlockingQueue(unsigned int size)
                :size(size), queue(), lock(), cond() {
        }
        //不可复制
        BlockingQueue(const BlockingQueue&) = delete;
        BlockingQueue &operator=(const BlockingQueue &) = delete;
        //默认移动构造不可用，因为mutex和cond不可移动

        ~BlockingQueue() = default;

        //将对象入队
        void Push(const T &);
        //返回队头对象智能指针并从队列中删除队头
        std::shared_ptr<T> Pop();
        //是否为空（非线程安全）
        bool Empty() const;

    private:
        unsigned int size;
        std::deque<T> queue;
        mutable std::mutex lock;
        std::condition_variable cond;
    };
}

#endif //SIMPLETON_BLOCKINGQUEUE_H
