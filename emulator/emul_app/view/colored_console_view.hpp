#pragma once

#include <iostream>
#include <string_view>
#include "app_view.hpp"
#include "log_message.hpp"
#include "log_type.hpp"
#include "console_color.hpp"

class ColoredConsoleView : public AppView {

    /// @see AppView::display_message(LogMessage&)
    void display_message(LogMessage& msg) const override
    {
        auto time_stamp = std::chrono::time_point_cast<std::chrono::seconds>(msg.getTimeStamp());
        std::string formatted_time = std::format("{0:%F %T}", time_stamp);
        std::string prefix = std::format("[{} - {:^7}] ", formatted_time, LogType::convert(msg.getMsgLvl().first).value_or(std::string_view()));

        ConsoleColor::color_t color = ConsoleColor::convert(msg.getMsgLvl()).value_or(ConsoleColor::values::FG_DEFAULT);

        std::cout << color.data() << prefix << msg.getMsg() << ConsoleColor::values::RESET << std::endl;
    }
};