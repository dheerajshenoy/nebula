#pragma once

#include <vector>

class util {

public:
    util() = delete;

    template <typename T>
    static void fast_swap(std::vector<T>& vec, const size_t &i, const size_t &j) noexcept {
        if (i != j) {
            T tmp = std::move(vec[i]);
            vec[i] = std::move(vec[j]);
            vec[j] = std::move(tmp);
        }
    }
};
