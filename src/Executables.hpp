#pragma once

#include <forward_list>
#include <string>
#include <experimental/filesystem>
#include <iostream>

namespace fs = std::experimental::filesystem;

class Executables {

    std::forward_list<std::string> entries_;

public:

    explicit Executables(const std::initializer_list<std::string> &list) {
        for (const auto &path : list) {
            for (const auto &f : fs::directory_iterator(path)) {
                entries_.push_front(f.path().filename());
            }
        }
    }

    bool find(const std::string &name) const;

};

