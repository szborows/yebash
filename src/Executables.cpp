#include "Executables.hpp"

bool Executables::find(std::string name) {
    for (const auto &e : entries_) {
        if (name == e)
            return true;
    }
    return false;
}
