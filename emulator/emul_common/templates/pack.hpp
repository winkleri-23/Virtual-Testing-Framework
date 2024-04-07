#pragma once

#include <concepts>

namespace pack {
template <typename First, typename Second>
struct packImpl {

    constexpr packImpl() = default;

    constexpr packImpl(First _first, Second _second)
        : first(_first)
        , second(_second) {};
    First first {};
    Second second {};

    constexpr auto operator<=>(const packImpl&) const = default;
    constexpr bool operator==(const packImpl&) const = default;

    constexpr auto operator<=>(const First& x) const { return first <=> x; };

    constexpr bool operator==(const First& x) const { return first == x; };

    constexpr auto operator<=>(const Second& x) const { return second <=> x; };

    constexpr bool operator==(const Second& x) const { return second == x; };

    // Type traits

    /// @brief Default false case
    template <typename T>
    struct to_oposite : std::false_type {
        using type = void;
    };

    /// Converts between type First and Second

    template <typename T>
        requires std::convertible_to<T, First>
    struct to_oposite<T> : std::true_type {
        using type = Second;
    };
    template <typename T>
        requires std::convertible_to<T, Second>
    struct to_oposite<T> : std::true_type {
        using type = First;
    };

    template <typename T>
    using to_oposite_t = typename to_oposite<T>::type;
    template <typename T>
    static constexpr auto to_oposite_v = to_oposite<T>::value;
};

/// @brief Converts packImpl structure between its two types
/// @tparam T Type to be converted to
/// @tparam S First packImpl type
/// @tparam N Second packImpl type
/// @param p _packImpl<S,N> instance
/// @return _packImpl value of type T, which either T == S or T == N
template <typename T, typename S, typename N>
    requires std::convertible_to<T, S> || std::convertible_to<T, N>
constexpr auto convert(packImpl<S, N> p) -> decltype(auto)
{
    if constexpr (std::convertible_to<T, S>)
        return p.first;
    if constexpr (std::convertible_to<T, N>)
        return p.second;
}

}