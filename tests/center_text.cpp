#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>

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

int main() {
    std::string text = "Hello, World!";
    print_centered(text);
    return 0;
}
