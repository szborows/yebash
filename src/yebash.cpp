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

#define cursor_forward(x) printf("\033[%dC", static_cast<int>(x))
#define cursor_backward(x) printf("\033[%dD", static_cast<int>(x))

typedef ssize_t (*ReadSignature)(int, void*, size_t);

thread_local ReadSignature realRead = NULL;

thread_local std::array<char, 1024> lineBuffer;
thread_local auto lineBufferPos = lineBuffer.begin();

thread_local std::vector<std::string> history;
thread_local auto historyCurrentPos = history.end(); // FIXME: this is wrong, but I only need the iterator type

static void readHistory() {

    if (history.size()) return;

    std::string historyFileName(getenv("HOME"));
    historyFileName += "/.bash_history";

    std::ifstream historyFile(historyFileName);
    std::string line;

    while (std::getline(historyFile, line))
        history.push_back(line); // TODO: maybe reverse order?

}

static void newlineHandler() {

    lineBuffer.fill(0);
    lineBufferPos = lineBuffer.begin();

}

static void backspaceHandler() {

    if (lineBufferPos != lineBuffer.begin())
        *(--lineBufferPos) = 0;

}

std::string findCompletion(const std::string &pattern) {

    for (auto it = history.end() - 1; it > history.begin(); it--) {
        if (it->compare(0, pattern.length(), pattern) == 0) {
            historyCurrentPos = it;
            return *it;
        }
    }

    historyCurrentPos = history.end() - 1;
    return pattern;

}


// TODO: merge this with the above function
std::string findNextCompletion(const std::string &pattern) {

    for (auto it = historyCurrentPos; it >= history.begin(); it--) {
        if (it->compare(0, pattern.length(), pattern) == 0) {
            historyCurrentPos = it;
            return *it;
        }
    }

    historyCurrentPos = history.end() - 1;
    return pattern;

}

void printCompletion() {

    std::string pattern(lineBuffer.data());
    auto completion = findCompletion(pattern);

    cursor_forward(1);
    printf("\e[1;30m%s\e[0m", completion.c_str() + pattern.length());
    cursor_backward(completion.length() - pattern.length() + 1);
    fflush(stdout);

}

void regularCharHandler(char c) {

    *lineBufferPos = c;
    lineBufferPos++;

    printCompletion();

}

void tabHandler() {

    printCompletion();

}

static unsigned char yebash(unsigned char c) {

    // TODO: uncomment later
    //if (!getenv("YEBASH"))
    //    return;

    readHistory();

    switch (c) {

        case 0x09: // tab
            tabHandler();
            break;

        case 0x0d: // newline
            newlineHandler();
            break;

        case 0x17: // ctrl+w
            newlineHandler(); // TODO: this has to clear lineBuffer
            break;

        case 0x7f: // backspace
            backspaceHandler();
            break;

        default: // regular char or other special chars
            if (c < 0x20)
                newlineHandler();
            else
                regularCharHandler(c);
            break;

    }

    return 0;

}

ssize_t read(int fd, void *buf, size_t count) {

    ssize_t returnValue;

    if (!realRead)
        realRead = reinterpret_cast<ReadSignature>(dlsym(RTLD_NEXT, "read"));

    returnValue = realRead(fd, buf, count);

    if (fd == 0 && isatty(fileno(stdin))) {
        unsigned char c = yebash(*reinterpret_cast<unsigned char *>(buf));
        if (c) *reinterpret_cast<unsigned char *>(buf) = c;
    }

    return returnValue;

}

