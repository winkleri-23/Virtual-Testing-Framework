#pragma once

#include <string_view>
#include "app_view.hpp"
#include "colored_console_view.hpp"
#include "console_view.hpp"
#include "log_type.hpp"
#include "logger.hpp"
#include "message_queue.hpp"

namespace viewhandle {
using namespace std::string_view_literals;

/// @brief App view identifier
const std::string_view APP_TAG = "app"sv;

/// @brief Dll view identifier
const std::string_view DLL_TAG = "dll"sv;

/// @brief AppView pointer to instance
extern AppView* view;

/// @brief Initializes default view
extern void init_default_view();

/// @brief Changes view to colored console view
extern void enable_console_color_view();

/// @brief Register message queue source to view
/// @param tag source identifier
/// @param msgSrc pointer to message queue instance
extern void register_msg_source(std::string_view tag, MessageQueue* msgSrc);

/// @brief Unregisters message source from view
/// @param tag source identifier
extern void unregister_msg_source(std::string_view tag);

/// @brief Sets message level for view
/// @param lvl message lvl string
/// @return bool if succefully set, otherwise false
extern bool set_msg_lvl(const char* lvl);

/// @brief Sets message level for view
/// @param lvl message lvl 
/// @return bool if succefully set, otherwise false
extern bool set_msg_lvl(LogType::type lvl);

/// @brief Runs the view 
extern void run();

/// @brief Stops the view run
extern void stop();

} // namespace viewhandle
