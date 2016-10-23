#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <termios.h>
#include <sys/ioctl.h>

#include "TerminalInfo.hpp"

int TerminalInfo::getCursorPosition() {
    int retVal = 0, x, y;
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
            retVal = y;

    tcsetattr(STDIN_FILENO, TCSADRAIN, &oldTermios);

    return retVal;
}

int TerminalInfo::getWidth() {
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    return w.ws_col;
}


