#pragma once

#include <array>
#include <concepts>
#include <string_view>
#include <utility>
#include "pack.hpp"

namespace LogType {

using namespace std::string_view_literals;

using num_t = uint8_t;
using str_t = std::string_view;

using type = pack::packImpl<num_t, str_t>;

// Values - must be unique values
constexpr type Debug { 0, "DEBUG"sv };
constexpr type Info { 1, "INFO"sv };
constexpr type Warning { 2, "WARNING"sv };
constexpr type Error { 3, "ERROR"sv };
constexpr std::array vals = { Debug, Info, Warning,
    Error };

/// @brief Convets between the bundled types in type
/// @tparam T either num_t or str_t type
/// @param t instance of T
/// @return std::optional, that when succefull contains the opposite value of the opposite type
template <typename T>
    requires(type::to_oposite_v<T>) && (std::convertible_to<T, str_t> || std::convertible_to<T, num_t>)
constexpr auto convert(T t) -> std::optional<type::to_oposite_t<T>>
{
    for (auto e : vals) {
        if (e == t) {
            return pack::convert<type::to_oposite_t<T>>(e);
        }
    }
    return std::nullopt;
}

/// @brief Gets parent instance of the two bundled types
/// @tparam T one of the bundled types
/// @param t instance of T
/// @return std::optional instance, that when succesfull contains the parent instance
template <typename T>
constexpr auto parent(T t) -> std::optional<type>
{
    for (auto e : vals) {
        if (e == t) {
            return e;
        }
    }
    return std::nullopt;
}

} // namespace LogType