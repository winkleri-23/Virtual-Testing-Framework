
#pragma once

#include <array>
#include <concepts>
#include <string_view>
#include <utility>
#include "pack.hpp"
#include <optional>



namespace DataObj {
    using namespace std::string_view_literals;

    using num_t = uint32_t;
    using str_t = std::string_view;

    using type = pack::packImpl<num_t, str_t>;

    constexpr num_t __Base = 0xDEAD0000;
    constexpr type IN_DATA{ __Base + 1, "input_data" };
    constexpr type OUT_DATA{ __Base + 2, "output_data" };
    constexpr type SET_DIAG{ __Base + 3, "set_diag" };
    constexpr type SPI_OUT{ __Base + 4, "spi_output_data" };
    constexpr type SPI_IN{ __Base + 5, "spi_input_data" };
    constexpr type POWER_SUPPLY{ __Base + 6, "power_supply" };
    constexpr type DIAG_BUFFER{ __Base + 7, "diag_buffer" };
    constexpr type PHYS_OUT_DATA{ __Base + 8, "phys_output_data" };
    constexpr type INVALID_VALUE{ __Base + 9, "" };

    std::array vals{
        IN_DATA,
        OUT_DATA,
        SET_DIAG,
        SPI_OUT,
        SPI_IN,
        POWER_SUPPLY,
        DIAG_BUFFER,
        PHYS_OUT_DATA,
    };

    /// @brief Gets a integral value from pack structure
    /// @return integral value
    template <typename T, typename S>
        requires(std::integral<T> || std::integral<S>)
    constexpr auto to_integral(pack::packImpl<T, S> t) -> decltype(auto)
    {
        if constexpr (std::integral<T>)
            return t.first;
        if constexpr (std::integral<S>)
            return t.second;
    }

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

}