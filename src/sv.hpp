#pragma once

#include <array>

namespace dj {
    class string_view {

    public:
        constexpr string_view() noexcept : _data(nullptr), _size(0) {}
        constexpr string_view(const char *str) : _data(str),
            _size(length(str)) {}
    constexpr string_view(const char *str, const size_t &len) : _data(str), _size(len) {}
    constexpr string_view(const std::string &str) : _data(str.data()), _size(str.size()) {}

    // Size functions
    inline constexpr size_t size() const noexcept { return _size; }
    inline constexpr size_t length() const noexcept { return _size; }
    inline constexpr bool empty() const noexcept { return _size == 0; }

    // Element access
    constexpr char operator[](const size_t &pos) const {
        return (pos < _size) ? _data[pos] : throw std::out_of_range("Index out of range");
    }

    inline constexpr char front() const { return _data[0]; }
    inline constexpr char back() const { return _data[_size - 1]; }
    inline constexpr const char* data() const noexcept { return _data; }

    // Substring
    constexpr MyStringView substr(const size_t &pos, const size_t &count = npos) const {
        if (pos > _size) throw std::out_of_range("pos out of range");
        return string_view(_data + pos, (count > _size - pos) ? _size - pos : count);
    }

    // Iterators
    inline constexpr const char* begin() const noexcept { return _data; }
    inline constexpr const char* end() const noexcept { return _data + _size; }

    // Comparison operators
    constexpr bool operator==(const MyStringView& other) const noexcept {
        return size_ == other.size_ && std::char_traits<char>::compare(data_, other.data_, size_) == 0;
    }

    constexpr bool operator!=(const MyStringView& other) const noexcept {
        return !(*this == other);
    }

    // Output stream support
    friend std::ostream& operator<<(std::ostream& os, const MyStringView& sv) {
        return os.write(sv.data_, sv.size_);
    }

    // Constants
    static constexpr size_t npos = -1;

    private:
        const char* _data;
        size_t _size;
        void len(const char *str);

    };

    // Returns the length of the string_view
    constexpr static size_t string_view::length(const char *str) {
        size_t len = 0;
        while (str && str[len] != '\0')
            ++len;
        return len;
    }
}
