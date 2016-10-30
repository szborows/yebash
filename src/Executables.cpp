#include "Executables.hpp"

bool Executables::find(const std::string &name) const {
    for (const auto &e : entries_) {
        if (name == e)
            return true;
    }
    return false;
}
