#pragma once

#include <string>
#include <vector>

namespace yb {

using HistoryEntries = std::vector<std::string>;

struct History final {
    void read(std::string const& path);

    using const_iterator = HistoryEntries::const_iterator;
    const_iterator begin() const { return entries_.cbegin(); };
    const_iterator end() const { return entries_.cend(); };

private:
    HistoryEntries entries_;
};

} // namespace yb

