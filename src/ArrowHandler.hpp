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

    std::array<std::function<void(HistorySuggestion &, Printer &)>, 4> handlers_;

};

} // namespace yb

