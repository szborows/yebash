#include <dlfcn.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <termios.h>
#include <sys/ioctl.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <string>
#include <map>
#include <functional>
#include <experimental/optional>

#include "History.hpp"

#define cursor_forward(x) printf("\033[%dC", static_cast<int>(x))
#define cursor_backward(x) printf("\033[%dD", static_cast<int>(x))

thread_local std::array<char, 1024> lineBuffer;
thread_local auto lineBufferPos = lineBuffer.begin();

thread_local std::string printBuffer;
thread_local std::string::iterator printBufferPos;

using namespace yb;
thread_local History history;
thread_local History::const_iterator historyPos;

static char arrowIndicator = 0;

using ReadSignature = ssize_t (*)(int, void*, size_t);
using Char = unsigned char;
using CharOpt = std::experimental::optional<Char>;

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
    {0x1b, arrowHandler1},
    {0x5b, arrowHandler2},
    {0x43, arrowHandler3},
    {0x7f, backspaceHandler}
};

int getCursorPosition(int &x, int &y) {
    int retVal = -1;
    fd_set stdInSet;
    struct timeval time;
    struct termios rawTermios, oldTermios;

    tcgetattr(STDIN_FILENO, &oldTermios);
    rawTermios = oldTermios;
    rawTermios.c_lflag &= ~ICANON;
    rawTermios.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &rawTermios);

    printf("\033[6n");
    fflush(stdout);

    FD_ZERO(&stdInSet);
    FD_SET(STDIN_FILENO, &stdInSet);
    time.tv_sec = 0;
    time.tv_usec = 100000;

    if (select(STDIN_FILENO + 1, &stdInSet, NULL, NULL, &time) == 1)
        if (scanf("\033[%d;%dR", &x, &y) == 2)
            retVal = 0;

    tcsetattr(STDIN_FILENO, TCSADRAIN, &oldTermios);

    return retVal;
}

int getTerminalWidth() {
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    return w.ws_col;
}

void clearTerminalLine() {
    // TODO: get info about terminal width and current cursor position
    // and fix below loops
    int col, row, width;
    if (getCursorPosition(row, col)) return;
    width = getTerminalWidth();
    for (int i = 0; i < width - col; i++)
        printf(" ");
    fflush(stdout);
    for (int i = 0; i < width - col; i++)
        cursor_backward(1);
}


std::string findCompletion(History::const_iterator start, const std::string &pattern) {
    for (auto it = start - 1; it > history.begin(); it--) {
        if (it->compare(0, pattern.length(), pattern) == 0) {
            historyPos = it;
            return *it;
        }
    }

    historyPos = history.end();
    return pattern;
}


void printCompletion(History::const_iterator startIterator, int offset) {
    std::string pattern(lineBuffer.data());
    auto completion = findCompletion(startIterator, pattern);

    clearTerminalLine();

    if (offset)
        cursor_forward(offset);
    printf("\e[1;30m%s\e[0m", completion.c_str() + pattern.length());

    cursor_backward(completion.length() - pattern.length() + offset);
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

    printCompletion(history.end(), 1);

    return {};
}

CharOpt tabHandler(Char) {
    printCompletion(historyPos, 0);
    return Char{0}; // TODO: this does not seem to work.
}

CharOpt arrowHandler1(Char) {
    arrowIndicator = 1;
    return {};
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
    }
    else {
        return_value = regularCharHandler(c);
    }
    try {
        printBuffer = historyPos->substr(lineBufferPos - lineBuffer.begin());
        printBufferPos = printBuffer.begin();
    } catch (...) {
        // FIXME:
    }

    return return_value;
}

static unsigned char yebash(unsigned char c) {

    // TODO: uncomment later
    //if (!getenv("YEBASH"))
    //    return;
    history.read(std::string{getenv("HOME")} + "/.bash_history");

    auto handler = handlers[c];

    // TODO(Mrokkk): There would be a problem with processing all escape codes
    // that bash is using (e.g. ctrl+r for history searching, arrows for moving, etc.).
    // It would be thoughtful if we just disable our yebash if any of these codes would
    // occur and reenable it after a newline
    CharOpt cReturned = c;
    if (handler) {
        handler(c);
    }
    else {
        if (c < 0x20) {
            newlineHandler(c);
        }
        else {
            regularCharHandler(c);
        }
    }

    if (static_cast<bool>(cReturned)) {
        return cReturned.value();
    }

    return c;
}

ssize_t read(int fd, void *buf, size_t count) {

    ssize_t returnValue;
    static thread_local ReadSignature realRead = nullptr;

    if (fd == 0) { // TODO: make it look good
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

    if (!realRead)
        realRead = reinterpret_cast<ReadSignature>(dlsym(RTLD_NEXT, "read"));

    returnValue = realRead(fd, buf, count);

    if (fd == 0 && isatty(fileno(stdin)))
        *reinterpret_cast<unsigned char *>(buf) = yebash(*reinterpret_cast<unsigned char *>(buf));

    return returnValue;
}


