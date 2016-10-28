#pragma once

#include <experimental/optional>
#include <string>

enum class Color {
    grey = 2,
    black = 30,
    red = 31,
    green = 32,
    yellow = 33,
    blue = 34,
    magenta = 35,
    cyan = 36,
    light_grey = 37,
    dark_grey = 90,
    light_red = 91,
    light_green = 92,
    light_yellow = 93,
    light_blue = 94,
    light_magenta = 95,
    light_cyan = 96,
    white = 97
};

using Char = unsigned char;
using CharOpt = std::experimental::optional<Char>;
using StringOpt = std::experimental::optional<std::string>;
using ColorOpt = std::experimental::optional<Color>;

