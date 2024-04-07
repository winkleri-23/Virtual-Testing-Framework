#include "dll_data.hpp"
#include "dll_message_queue.hpp"

DllMessageQueue* dll_data::dllMsgQueue = nullptr;

void dll_data::init_queue(EMUL_DLL_GET_MSG_LEN_FUNC getLenFunc, EMUL_DLL_GET_MSG_FUNC getMsgFunc)
{
    dll_data::dllMsgQueue = new DllMessageQueue(getLenFunc, getMsgFunc);
}

void dll_data::deinit_queue()
{
    if (dll_data::dllMsgQueue != nullptr) {
        delete dll_data::dllMsgQueue;
        dll_data::dllMsgQueue = nullptr;
    }
}
