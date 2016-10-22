#include "History.hpp"

#include <fstream>
#include <stdexcept>

using namespace std;

namespace yb {

void History::read(std::string const& path) {
    if (!entries_.empty()) {
        return;
    }

    ifstream historyFile(path);
    if (!historyFile.is_open()) {
        throw runtime_error{"Could not open history file"};
    }

    string line;
    while (getline(historyFile, line)) {
        entries_.push_back(line); // TODO: maybe reverse order?
    }
}

} // namespace yb
