#pragma once

#include <experimental/optional>

using Char = unsigned char;
using CharOpt = std::experimental::optional<Char>;
using StringOpt = std::experimental::optional<std::string>;

enum class Color {
    grey = 2,
    black = 30,
    red = 31,
    green = 32,
    yellow = 33,
    blue = 34,
    magenta = 35,
    cyan = 36
};

