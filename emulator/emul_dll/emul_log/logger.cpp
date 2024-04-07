#pragma once

#include "logger.hpp"

MessageQueue Logger::msgQueue;

MessageQueue* Logger::getMsgQueue() {
    return &Logger::msgQueue;
}