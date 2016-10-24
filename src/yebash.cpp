#include <dlfcn.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <cstdlib>

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <string>
#include <map>
#include <functional>
#include <stdexcept>

#include "yebash.hpp"
#include "Defs.hpp"
#include "TerminalInfo.hpp"
#include "KeyHandlers.hpp"

#define cursor_forward(x) printf("\033[%dC", static_cast<int>(x))
#define cursor_backward(x) printf("\033[%dD", static_cast<int>(x))

// https://www.akkadia.org/drepper/tls.pdf

using namespace yb;

thread_local std::array<char, 1024> lineBuffer;
thread_local auto lineBufferPos = lineBuffer.begin();

thread_local std::string printBuffer;
thread_local std::string::iterator printBufferPos;

thread_local History history;
thread_local History::const_iterator historyPos;

thread_local char arrowIndicator = 0;

using ReadSignature = ssize_t (*)(int, void*, size_t);
static thread_local ReadSignature realRead = nullptr;

CharOpt newlineHandler(History const&, Char);
CharOpt tabHandler(History const&, Char);
CharOpt backspaceHandler(History const&, Char);
CharOpt regularCHarHandler(History const&, Char);
CharOpt arrowHandler1(History const&, Char);
CharOpt arrowHandler2(History const&, Char);
CharOpt arrowHandler3(History const&, Char);

thread_local std::map<Char, std::function<CharOpt(History const&, Char)>> handlers = {
    {0x06, tabHandler},
    {0x0d, newlineHandler},
    {0x17, newlineHandler}, // TODO: this should delete one word
    {0x1b, yb::arrowHandler1},
    {0x5b, arrowHandler2},
    {0x43, arrowHandler3},
    {0x7f, backspaceHandler}
};

static inline void deleteRows(int rows) {
    for (int i = 0; i < rows; i++)
        std::putchar(' ');
    for (int i = 0; i < rows; i++)
        cursor_backward(1);
    fflush(stdout);
}

void clearTerminalLine() {
    int pos, width;
    if (!(pos = TerminalInfo::getCursorPosition())) return;
    width = TerminalInfo::getWidth();
    deleteRows(width - pos);
}

StringOpt findCompletion(History::const_iterator start, const std::string &pattern) {
    for (auto it = start; it != history.end(); it++) {
        if (it->compare(0, pattern.length(), pattern) == 0) {
            historyPos = it;
            return *it;
        }
    }
    historyPos = history.begin();
    return {};
}

static inline void printColor(const char *buffer) {
    printf("\e[31m%s\e[0m", buffer);
}

void printCompletion(History::const_iterator startIterator, int offset) {
    std::string pattern(lineBuffer.data());
    auto completion = findCompletion(startIterator, pattern);
    if (!completion) {
        return;
    }
    if (pattern.length() == completion.value().length()) {
        return;
    }
    clearTerminalLine();
    if (offset)
        cursor_forward(offset);
    printColor(completion.value().c_str() + pattern.length());
    cursor_backward(completion.value().length() - pattern.length() + offset);
    fflush(stdout);
}

CharOpt newlineHandler(History const&, Char) {
    lineBuffer.fill(0);
    lineBufferPos = lineBuffer.begin();
    return {};
}

CharOpt backspaceHandler(History const&, Char) {
    if (lineBufferPos != lineBuffer.begin()) {
        *(--lineBufferPos) = 0;
    }
    return {};
}

CharOpt regularCharHandler(History const& history, Char c) {
    *lineBufferPos = c;
    lineBufferPos++;
    printCompletion(history.begin(), 1);
    return {};
}

CharOpt tabHandler(History const&, Char) {
    printCompletion(std::next(historyPos, 1), 0);
    return Char{0}; // TODO: this does not seem to work.
}

CharOpt arrowHandler2(History const& history, Char c) {
    if (arrowIndicator == 1) {
        arrowIndicator = 2;
        return {};
    }
    else {
        return regularCharHandler(history, c);
    }
}

CharOpt arrowHandler3(History const& history, Char c) {
    CharOpt return_value = {};
    if (arrowIndicator == 2) {
        arrowIndicator = 0;
        try {
            printBuffer = historyPos->substr(lineBufferPos - lineBuffer.begin());
            printBufferPos = printBuffer.begin();
        } catch (...) {
            // FIXME:
        }
    }
    else {
        return_value = regularCharHandler(history, c);
    }
    return return_value;
}

namespace yb {

unsigned char yebash(History const& history, unsigned char c) {
    // TODO: uncomment later
    //if (!getenv("YEBASH"))
    //    return;
    auto handler = handlers[c];
    CharOpt cReturned;
    if (handler) {
        cReturned = handler(history, c);
    }
    else {
        if (c < 0x20) {
            newlineHandler(history, c);
        }
        else {
            regularCharHandler(history, c);
        }
    }
    return cReturned.value_or(c);
}

} // namespace yb

static inline bool is_terminal_input(int fd) {
    return isatty(fd);
}

static inline void putCharToReadBuffer(char *buf) {
    *buf = *printBufferPos;
    *lineBufferPos++ =  *printBufferPos++;
    if (printBufferPos == printBuffer.end()) {
        printBuffer.erase(printBuffer.begin(), printBuffer.end());
    }
}

ssize_t read(int fd, void *buf, size_t count) {
    if (is_terminal_input(fd) && printBuffer.length()) {
        putCharToReadBuffer(static_cast<char *>(buf));
        return 1;
    }
    auto returnValue = realRead(fd, buf, count);
    if (is_terminal_input(fd)) {
        *static_cast<unsigned char *>(buf) = yb::yebash(history, *static_cast<unsigned char *>(buf));
    }
    return returnValue;
}

__attribute__((constructor))
static void yebashInit()  {
    if (!realRead) {
        realRead = reinterpret_cast<ReadSignature>(dlsym(RTLD_NEXT, "read"));
    }

    std::ifstream historyFile(std::string{getenv("HOME")} + "/.bash_history");
    if (!historyFile.is_open()) {
        throw std::runtime_error{"Could not open history file"};
    }
    history.read(historyFile);
}
