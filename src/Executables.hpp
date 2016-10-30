#pragma once

#include <list>
#include <string>
#include <experimental/filesystem>
#include <iostream>

namespace fs = std::experimental::filesystem;

class Executables {

    std::list<std::string> entries_;

public:

    explicit Executables(const std::initializer_list<std::string> &list) {
        for (const auto &path : list) {
            for (const auto &f : fs::directory_iterator(path)) {
                entries_.push_back(f.path().filename());
            }
        }
    }

    bool find(std::string name);

};

