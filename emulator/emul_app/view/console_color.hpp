#pragma once

#include <string_view>
#include "log_type.hpp"
#include "pack.hpp"

namespace ConsoleColor {
using namespace std::string_view_literals;

/// @brief integral reference value 
using val_t = LogType::type;

/// @brief color representation
using color_t = std::string_view;

/// @brief define of values, must be unique values
namespace values {
    using type = color_t;
// prefix string
#define _prefix "\033["
// postfix string
#define _postfix "m"
    constexpr type RESET { std::string_view { _prefix "0"sv _postfix} };
    constexpr type FG_RED { std::string_view { _prefix "31"sv _postfix} };
    constexpr type FG_GREEN { std::string_view { _prefix "32"sv _postfix} };
    constexpr type FG_YELLOW { std::string_view { _prefix "33"sv _postfix} };
    constexpr type FG_BLUE { std::string_view { _prefix "34"sv _postfix} };
    constexpr type FG_DEFAULT { std::string_view { _prefix "37"sv _postfix} };
    constexpr type BG_RED { std::string_view { _prefix "41"sv _postfix} };
    constexpr type BG_GREEN { std::string_view { _prefix "42"sv _postfix} };
    constexpr type BG_BLUE { std::string_view { _prefix "44"sv _postfix} };
    constexpr type BG_DEFAULT { std::string_view { _prefix "49"sv _postfix} };
}

/// @brief mapping for searching inbetween both values
namespace mapping {
    using type = pack::packImpl<LogType::type, color_t>;
    constexpr std::array vals {
        type { LogType::Debug, values::FG_DEFAULT },
        type { LogType::Info, values::FG_DEFAULT },
        type { LogType::Warning, values::FG_YELLOW },
        type { LogType::Error, values::FG_RED },
    };

}

/// @brief Converts values between val_t and color_t
/// @tparam T val_t or color_t
/// @param t instance of T
/// @return std::optional, that if contained in mapping::vals, contains the counter value to the T value 
template <typename T>
    requires(mapping::type::to_oposite_v<T>) && (std::convertible_to<T, color_t> || std::convertible_to<T, val_t>)
constexpr auto convert(T t) -> std::optional<mapping::type::to_oposite_t<T>>
{
    for (auto e : mapping::vals) {
        if (e == t) {
            return pack::convert<mapping::type::to_oposite_t<T>>(e);
        }
    }
    return std::nullopt;
}
}