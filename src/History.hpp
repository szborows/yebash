#pragma once

#include <string>
#include <forward_list>
#include <istream>
#include "Defs.hpp"

namespace yb {

struct History final {

    using HistoryEntries = std::forward_list<std::string>;
    using const_iterator = HistoryEntries::const_iterator;

    auto &getCurrentEntry() const { return position_; };

    void read(std::istream & input);
    StringOpt findCompletion(const std::string &pattern);
    StringOpt findNextCompletion(const std::string &pattern);

private:

    StringOpt find(const std::string &pattern, const_iterator start);

    HistoryEntries entries_;
    const_iterator position_;
};

} // namespace yb

