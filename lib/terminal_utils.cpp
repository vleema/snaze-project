#include "terminal_utils.h"
#include <cstring>
#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>

struct termios orig_termios;

void reset_terminal_mode() { tcsetattr(0, TCSANOW, &orig_termios); }

void set_terminal_mode() {
    struct termios new_termios;
    tcgetattr(0, &orig_termios);
    memcpy(&new_termios, &orig_termios, sizeof(new_termios));
    atexit(reset_terminal_mode);
    cfmakeraw(&new_termios);
    tcsetattr(0, TCSANOW, &new_termios);
}

int kbhit() {
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

int getch() {
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        return c;
    }
}

int get_terminal_width() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}

void print_centered(const std::string &text) {
    int terminal_width = get_terminal_width();
    int text_length = text.length();
    int padding = (terminal_width - text_length) / 2;

    if (padding > 0) {
        std::cout << std::string(padding, ' ') << text << std::endl;
    } else {
        std::cout << text << std::endl; // Text is too long to center
    }
}
