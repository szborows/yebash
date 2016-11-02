#pragma once

#include "Defs.hpp"
#include "HistorySuggestion.hpp"
#include "Printer.hpp"
#include <array>
#include <functional>

namespace yb {

class ArrowHandler {

    const int left_ = 0;
    const int up_ = 1;
    const int right_ = 2;
    const int down_ = 3;

    using Handler = void(HistorySuggestion &, Printer &);
    std::array<std::function<Handler>, 4> handlers_;
    std::array<std::string, 4> escapeCodes = {"\e[1D", "\e[1A", "\e[1C", "\e[1B"};
    std::array<Char, 4> currentState;

public:

    explicit ArrowHandler(Handler left, Handler up, Handler right, Handler down) {
        handlers_[left_] = left;
        handlers_[up_] = up;
        handlers_[right_] = right;
        handlers_[down_] = down;
    } 

};

} // namespace yb

