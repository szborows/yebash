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

#include "yebash.hpp"
#include "Defs.hpp"
#include "History.hpp"
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

CharOpt newlineHandler(Char);
CharOpt tabHandler(Char);
CharOpt backspaceHandler(Char);
CharOpt regularCHarHandler(Char);
CharOpt arrowHandler1(Char);
CharOpt arrowHandler2(Char);
CharOpt arrowHandler3(Char);

thread_local std::map<Char, std::function<CharOpt(Char)>> handlers = {
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

CharOpt newlineHandler(Char) {
    lineBuffer.fill(0);
    lineBufferPos = lineBuffer.begin();
    return {};
}

CharOpt backspaceHandler(Char) {
    if (lineBufferPos != lineBuffer.begin()) {
        *(--lineBufferPos) = 0;
    }
    return {};
}

CharOpt regularCharHandler(Char c) {
    *lineBufferPos = c;
    lineBufferPos++;
    printCompletion(history.begin(), 1);
    return {};
}

CharOpt tabHandler(Char) {
    printCompletion(std::next(historyPos, 1), 0);
    return Char{0}; // TODO: this does not seem to work.
}

CharOpt arrowHandler2(Char c) {
    if (arrowIndicator == 1) {
        arrowIndicator = 2;
        return {};
    }
    else {
        return regularCharHandler(c);
    }
}

CharOpt arrowHandler3(Char c) {
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
        return_value = regularCharHandler(c);
    }
    return return_value;
}

namespace yb {

unsigned char yebash(unsigned char c) {
    // TODO: uncomment later
    //if (!getenv("YEBASH"))
    //    return;
    history.read(std::string{getenv("HOME")} + "/.bash_history");
    auto handler = handlers[c];
    CharOpt cReturned;
    if (handler) {
        cReturned = handler(c);
    }
    else {
        if (c < 0x20) {
            newlineHandler(c);
        }
        else {
            regularCharHandler(c);
        }
    }
    return cReturned.value_or(c);
}

} // namespace yb

static inline bool is_terminal_input(int fd) {
    return isatty(fd);
}

ssize_t read(int fd, void *buf, size_t count) {
    if (is_terminal_input(fd)) { // TODO: make it look good
        if (printBuffer.length()) {
            // Return printBuffer to bash one char at time
            *reinterpret_cast<char *>(buf) = *printBufferPos;
            *lineBufferPos++ =  *printBufferPos++;
            if (printBufferPos == printBuffer.end()) {
                printBuffer.erase(printBuffer.begin(), printBuffer.end());
            }
            return 1;
        }
    }
    auto returnValue = realRead(fd, buf, count);
    if (is_terminal_input(fd)) {
        *reinterpret_cast<unsigned char *>(buf) = yb::yebash(*reinterpret_cast<unsigned char *>(buf));
    }
    return returnValue;
}

__attribute__((constructor))
static void yebashInit()  {
    if (!realRead) {
        realRead = reinterpret_cast<ReadSignature>(dlsym(RTLD_NEXT, "read"));
    }
}
