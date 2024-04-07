#pragma once
#include <mutex>
#include <queue>

#include "emul_concepts.hpp"
#include "log_message.hpp"
#include "log_type.hpp"

class MessageQueue {

public:
    MessageQueue() = default;

    MessageQueue(MessageQueue& other) = delete;

    void operator=(const MessageQueue&) = delete;

    template <typename T>
        requires emul_concepts::printable<T>
    void push(LogType::type msgLvl, T message)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        _msgQueue.emplace(msgLvl, message);
    }

    virtual void pop()
    {
        std::lock_guard<std::mutex> lock(_mtx);
        _msgQueue.pop();
    }

    virtual LogMessage& front() {
        return _msgQueue.front();
    }

    virtual bool isEmpty()
    {
        return _msgQueue.empty();
    }

    bool hasItems()
    {
        return !isEmpty();
    }

protected:
    std::mutex _mtx;

    std::queue<LogMessage> _msgQueue;
};
