#pragma once

#include <string>
#include <forward_list>
#include <istream>

namespace yb {

using HistoryEntries = std::forward_list<std::string>;

struct History final {
    void read(std::istream & input);

    using const_iterator = HistoryEntries::const_iterator;
    auto begin() const { return entries_.cbegin(); };
    auto end() const { return entries_.cend(); };

private:
    HistoryEntries entries_;
};

} // namespace yb

