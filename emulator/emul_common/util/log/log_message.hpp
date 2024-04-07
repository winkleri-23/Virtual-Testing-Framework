#pragma once

#include <chrono>
#include <exception>
#include <iostream>
#include <string>
#include "emul_concepts.hpp"
#include "emul_dll.h"
#include "log_type.hpp"

class LogMessage {

public:
    template <typename T>
        requires emul_concepts::printable<T>
    LogMessage(const LogType::type& lvl, const T& message)
        : msgLvl(lvl)
    {

        timeStamp = std::chrono::system_clock::now();

        std::ostringstream outStream;
        outStream << message;
        msg = outStream.str();
    }

    LogMessage(const LogType::type& lvl, const std::string& message)
        : msgLvl(lvl)
    {

        timeStamp = std::chrono::system_clock::now();
        msg = message;
    }

    LogMessage(const LogType::type& lvl, std::string&& message)
        : msgLvl(lvl)
    {

        timeStamp = std::chrono::system_clock::now();
        msg = std::move(message);
    }

    LogMessage(const LogType::type& lvl, char* message, std::chrono::system_clock::rep duration)
        : msgLvl(lvl)
    {
        auto tmp = std::chrono::system_clock::time_point { std::chrono::system_clock::duration { duration } };
        timeStamp = tmp;
        msg = message;
    }

    LogMessage(const LogType::type& lvl, char* message)
        : msgLvl(lvl)
    {
        timeStamp = std::chrono::system_clock::now();
        msg = std::string(message);
    }

    LogType::type getMsgLvl() const
    {
        return msgLvl;
    }

    std::string getMsg() const
    {
        return msg;
    }

    std::chrono::time_point<std::chrono::system_clock> getTimeStamp()
    {
        return timeStamp;
    }

    /// @brief Serializes message to uint8_t array
    /// @param outArr out array, to which the messsage will be serialized into
    /// @param arrLen out array length
    /// @return -1 if out array size is insufficient,
    ///         otherwise length of the serialized message
    std::optional<size_t> serialize(uint8_t* outArr, size_t arrLen)
    {
        int32_t len = getSerializedLength();
        if (static_cast<size_t>(len) > arrLen) {
            return std::nullopt;
        }

        SERIALIZED_MSG_HEADER msgHeader;
        msgHeader.msgType = msgLvl.first;
        msgHeader.timeStamp = timeStamp.time_since_epoch().count();

        std::memcpy(outArr, &msgHeader, sizeof(msgHeader));
        std::memcpy(outArr + sizeof(msgHeader), msg.c_str(), msg.size() + 1);
        return len;
    }

    /// @brief Deserializes message from array
    /// @param data data array
    /// @param len lenght of the passed data passed
    /// @return LogMessage instance
    static LogMessage deserialize(uint8_t* data, size_t len)
    {
        if (len < sizeof(SERIALIZED_MSG_HEADER))
            throw std::invalid_argument("Invalid data passed");

        SERIALIZED_MSG_HEADER header;
        std::memcpy(&header, data, sizeof(header));
        char* ptr = reinterpret_cast<char*>(data + sizeof(header));
        return LogMessage(LogType::parent(header.msgType).value_or(LogType::Error), ptr, header.timeStamp);
    }

    size_t getSerializedLength() {
        return sizeof(SERIALIZED_MSG_HEADER) + msg.size() + 1;
    }

#pragma pack(push, 1)
    typedef struct _SERIALIZED_MSG_HEADER_S {

        LogType::num_t msgType;
        std::chrono::system_clock::rep timeStamp;

    } SERIALIZED_MSG_HEADER;
#pragma pack(pop)

private:
    LogType::type msgLvl;

    std::string msg;

    std::chrono::time_point<std::chrono::system_clock> timeStamp;
};