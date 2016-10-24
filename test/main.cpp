#include "yebash.hpp"
#include "History.hpp"

#include <iostream>
#include <sstream>
using namespace std;

using namespace yb;

int main() {
    std::stringstream ss;
    ss << "a command-1\n";
    ss << "b command-1\n";
    ss << "c command-1\n";

    History history;
    history.read(ss);
    auto c = yebash(history, 'd');
    cout << c << endl;
}
