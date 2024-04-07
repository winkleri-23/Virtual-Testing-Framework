#pragma once
#include <iostream>

namespace emul_concepts {

template <typename T>
concept printable = requires(T t, std::ostream& os) {
    {
        os << t
    };
};

}