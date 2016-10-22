#include <dlfcn.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <termios.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <string>
#include <map>
#include <functional>

#define cursor_forward(x) printf("\033[%dC", static_cast<int>(x))
#define cursor_backward(x) printf("\033[%dD", static_cast<int>(x))

typedef ssize_t (*ReadSignature)(int, void*, size_t);

thread_local std::array<char, 1024> lineBuffer;
thread_local auto lineBufferPos = lineBuffer.begin();

thread_local std::string printBuffer;
thread_local std::string::iterator printBufferPos;

thread_local std::vector<std::string> history;
thread_local std::vector<std::string>::iterator historyPos;

static char arrowIndicator = 0;

void newlineHandler(unsigned char &);
void tabHandler(unsigned char &);
void backspaceHandler(unsigned char &);
void regularCHarHandler(unsigned char &);
void arrowHandler1(unsigned char &c);
void arrowHandler2(unsigned char &c);
void arrowHandler3(unsigned char &c);

thread_local std::map<unsigned char, std::function<void(unsigned char &)>> handlers = {
    {0x06, tabHandler},
    {0x0d, newlineHandler},
    {0x17, newlineHandler}, // TODO: this should delete one word
    {0x1b, arrowHandler1},
    {0x5b, arrowHandler2},
    {0x43, arrowHandler3},
    {0x7f, backspaceHandler}
};

static void readHistory() {

    if (history.size()) return;

    std::string historyFileName(getenv("HOME"));
    historyFileName += "/.bash_history";

    std::ifstream historyFile(historyFileName);
    std::string line;

    if (!historyFile.is_open()) {
        fprintf(stderr, "Could not open history file!");
        exit(1);
    }

    while (std::getline(historyFile, line))
        history.push_back(line); // TODO: maybe reverse order?

}

void newlineHandler(unsigned char &c) {

    (void)c;

    lineBuffer.fill(0);
    lineBufferPos = lineBuffer.begin();

}

void backspaceHandler(unsigned char &c) {

    (void)c;

    if (lineBufferPos != lineBuffer.begin())
        *(--lineBufferPos) = 0;

}

std::string findCompletion(std::vector<std::string>::iterator start, const std::string &pattern) {

    for (auto it = start - 1; it > history.begin(); it--) {
        if (it->compare(0, pattern.length(), pattern) == 0) {
            historyPos = it;
            return *it;
        }
    }

    historyPos = history.end();
    return pattern;

}

void getCursorPosition(int &row, int &col) {

    char buffer[16], consoleCode[] = "\033[6n";
    termios old, raw;

    tcgetattr(0, &old);
    cfmakeraw(&raw);
    tcsetattr(0,TCSANOW,&raw);
    write(1, consoleCode, sizeof(consoleCode));
    read (0, buffer, sizeof(buffer));
    tcsetattr(0, TCSANOW, &old);

    row = buffer[2];
    col = 0; // TODO

}

void clearTerminalLine() {
    // TODO: get info about terminal width and current cursor position
    // and fix below loops
    int col, row;
    getCursorPosition(row, col);
    for (int i = 0; i < 30; i++)
        printf(" ");
    for (int i = 0; i < 30; i++)
        cursor_backward(1);

}

void printCompletion(std::vector<std::string>::iterator startIterator, int offset) {

    std::string pattern(lineBuffer.data());
    auto completion = findCompletion(startIterator, pattern);

    clearTerminalLine();

    if (offset)
        cursor_forward(offset);
    printf("\e[1;30m%s\e[0m", completion.c_str() + pattern.length());

    cursor_backward(completion.length() - pattern.length() + offset);
    fflush(stdout);

}

void regularCharHandler(unsigned char &c) {

    *lineBufferPos = c;
    lineBufferPos++;

    printCompletion(history.end(), 1);

}

void tabHandler(unsigned char &c) {

    printCompletion(historyPos, 0);
    c = 0; // TODO: this does not seem to work

}

void arrowHandler1(unsigned char &c) {
    (void)c;
    arrowIndicator = 1;
}

void arrowHandler2(unsigned char &c) {
    (void)c;
    if (arrowIndicator == 1)
        arrowIndicator = 2;
    else regularCharHandler(c);
}

void arrowHandler3(unsigned char &c) {
    (void)c;
    if (arrowIndicator == 2) {
        arrowIndicator = 0;
    } else regularCharHandler(c);
    try {
        printBuffer = historyPos->substr(lineBufferPos - lineBuffer.begin());
        printBufferPos = printBuffer.begin();
    } catch (...) {
        // FIXME:
    }
}

static unsigned char yebash(unsigned char c) {

    unsigned char cReturned = c;
    // TODO: uncomment later
    //if (!getenv("YEBASH"))
    //    return;

    readHistory();

    auto handler = handlers[c];

    if (handler)
        handler(cReturned);
    else {
        if (c < 0x20)
            newlineHandler(cReturned);
        else
            regularCharHandler(cReturned);
    }

    return cReturned;

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

