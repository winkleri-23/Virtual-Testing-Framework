#pragma once

#include <functional>
#include <thread>
#include <vector>
#include "log_message.hpp"
#include "message_queue.hpp"
#include "view_conf.hpp"

class AppView {
public:
    ~AppView()
    {
        if (t.joinable()) {
            stop();
        }
    }

    /// @brief Virtual function for displaying/handeling each message
    /// @param msg msg to be processed
    virtual void display_message(LogMessage& msg) const = 0;

    /// @brief Main function that handles each view cycle
    ///        Runs on separate thread
    void _handle_run()
    {
        // init buffer for messages
        std::vector<LogMessage> buffer;
        buffer.reserve(5);

        while (is_running || hasMessages()) {

            // extra scope for the lock_guard, for correct release of the aquired lock
            {
                // aquire mutex during a cycle
                std::lock_guard<std::mutex> lock(_mtx);

                // check message sources and put incomming messages into buffer
                for (auto& [key, msgQueue] : viewConf.msgSrcs) {
                    while (msgQueue->hasItems()) {
                        if (msgQueue->front().getMsgLvl() >= viewConf.curLogLvl) {
                            auto msg = msgQueue->front();
                            buffer.push_back(msg);
                        }
                        msgQueue->pop();
                    }
                }
            }

            // thread sleep if buffer is empty
            if (buffer.empty()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                continue;
            }

            // if there are messages, sort them by timestamp
            if (buffer.size() > 1) {
                std::sort(buffer.begin(), buffer.end(),
                    [](LogMessage A, LogMessage B) {
                        return (A.getTimeStamp() < B.getTimeStamp());
                    });
            }

            // display all messages
            for (auto& it : buffer)
                this->display_message(it);

            // clear the buffer
            buffer.clear();
        }
    }

    /// @brief Starts the view run on worker thread
    void run()
    {
        if (is_running)
            return;

        is_running = true;
        t = std::thread(&AppView::_handle_run, this);
    }

    /// @brief Stops view run and joins the worker thread
    void stop()
    {
        is_running = false;
        if (t.joinable()) {
            t.join();
        }
    }

    /// @brief Registers message source to view
    /// @param tag identifier tag
    /// @param queue message queue instance pointer
    void registerSource(std::string_view tag, MessageQueue* queue)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        viewConf.msgSrcs.emplace(tag, queue);
    }

    /// @brief Unregisters message source
    /// @param tag identifier tag
    void unregisterSource(std::string_view tag)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        viewConf.msgSrcs.erase(tag);
    }

    /// @brief Sets current message level
    /// @param lvl msg level
    void setMsgLvl(LogType::type lvl)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        viewConf.curLogLvl = lvl;
    }

    /// @brief Sets view configuration
    /// @param viewConf view configuration instance
    void setViewConf(ViewConf viewConf)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        this->viewConf = viewConf;
    }

    /// @brief Gets view confuration
    /// @return view configuration instance
    ViewConf getViewConf() const
    {
        return viewConf;
    }

private:
    /// @brief Checks if any message source has messages available
    /// @return true if there messages to be processed, otherwise false
    bool hasMessages()
    {
        for (auto& [key, msgQueue] : viewConf.msgSrcs) {
            if (msgQueue->hasItems())
                return true;
        }
        return false;
    }

    /// @brief Mutex for access handeling
    std::mutex _mtx;

    /// @brief Flag for worker thread cycle
    bool is_running = false;

    /// @brief Worker thread instance
    std::thread t;

    /// @brief View configuration
    ViewConf viewConf;
};
