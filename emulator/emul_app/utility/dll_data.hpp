#pragma once

#include "dll_message_queue.hpp"
#include "emul_dll.h"

namespace dll_data {

/// @brief Initializes Dll message queue
/// @param getLenFunc Get message length function
/// @param getMsgFunc Get message function
extern void init_queue(EMUL_DLL_GET_MSG_LEN_FUNC getLenFunc, EMUL_DLL_GET_MSG_FUNC getMsgFunc);

/// @brief Deinits DLL queue
extern void deinit_queue();

/// @brief DllMessageQueue pointer to instance
extern DllMessageQueue* dllMsgQueue;
}