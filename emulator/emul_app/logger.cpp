#pragma once

#include "logger.hpp"
#include "dll_message_queue.hpp"

MessageQueue Logger::msgQueue;

MessageQueue* Logger::getMsgQueue() {
    return &Logger::msgQueue;
}