#pragma once

#include <map>
#include <string_view>
#include "log_type.hpp"
#include "message_queue.hpp"

struct ViewConf {
    LogType::type curLogLvl = LogType::Info;
    std::map<std::string_view, MessageQueue*> msgSrcs;
};
