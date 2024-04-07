#pragma once

#include <functional>
#include <memory>
#include <new>
#include "emul_dll.h"
#include "log_message.hpp"
#include "message_queue.hpp"
#include "logger.hpp"

class DllMessageQueue : public MessageQueue {
public:
    DllMessageQueue() = delete;

    DllMessageQueue(const DllMessageQueue&) = delete;

    auto operator=(const DllMessageQueue&) = delete;


    /// @brief Constructor
    /// @param getFunction Get Message function 
    DllMessageQueue(EMUL_DLL_GET_MSG_LEN_FUNC getLenFunction, EMUL_DLL_GET_MSG_FUNC getFunction): 
        getLenFunc(getLenFunction), getMsgFunc(getFunction) {}

    ~DllMessageQueue()
    {
        if (_buffer != nullptr)
            delete[] _buffer;
    }

    virtual bool isEmpty() override
    {
        return _msgQueue.empty() && getLenFunc() == 0;
    }

    virtual LogMessage& front() override {
        if (_msgQueue.empty()){
            std::lock_guard<std::mutex> lock(_mtx);
            pullDllQueue();
        }
        return _msgQueue.front();
    }

    bool hasItems()
    {
        return !isEmpty();
    }

private:
    /// @brief Pulls messages from DLL message queue
    void pullDllQueue()
    {
        DLL_GET_MSG_RESP_CODE resp = DLL_GET_MSG_RESP_CODE::MSG_OK;

        while (resp == DLL_GET_MSG_RESP_CODE::MSG_OK) {

            auto len = static_cast<size_t>(getLenFunc());

            if (len == 0)
                break;

            if (len > _size)
                allocate(len);

            resp = getMsgFunc(_buffer, _size);

            if (resp != DLL_GET_MSG_RESP_CODE::MSG_OK)
            {
                Logger::log(LogType::Error, "Unknow error when getting message from DLL");
            }

            _msgQueue.push(LogMessage::deserialize(_buffer, len));
        }
    }

    /// @brief Alocates buffer array
    /// @param newSize newSize of the array
    void allocate(size_t newSize)
    {
        if (_buffer != nullptr) {
            delete[] _buffer;
        }
        _buffer = new uint8_t[newSize];
        _size = newSize;
    }

    /// @brief Default buffer array size
    const size_t DEFAULT_SIZE = 75;

    /// @brief Get Dll message function
    EMUL_DLL_GET_MSG_LEN_FUNC getLenFunc;

    /// @brief Get Dll message function
    EMUL_DLL_GET_MSG_FUNC getMsgFunc;

    /// @brief Pointer to buffer array
    uint8_t* _buffer = nullptr;

    /// @brief Size of alocated buffer array
    size_t _size = 0;
};