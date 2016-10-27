#pragma once

#include <string>
#include <forward_list>
#include <istream>
#include "Defs.hpp"

namespace yb {

struct History final {

    using HistoryEntries = std::forward_list<std::string>;
    using const_iterator = HistoryEntries::const_iterator;

    auto begin() const { return entries_.cbegin(); }
    auto end() const { return entries_.cend(); }

    void read(std::istream & input);

private:

    HistoryEntries entries_;
};

} // namespace yb

