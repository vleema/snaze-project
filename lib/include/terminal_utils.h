#ifndef TERMINAL_UTILS_H
#define TERMINAL_UTILS_H

#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

extern struct termios orig_termios;

/**
 * @brief Resets the terminal to its original state.
 *
 * This function restores the terminal settings to the state they were in
 * before the program was started, typically used when exiting the program.
 */
inline void reset_terminal_mode() { tcsetattr(0, TCSANOW, &orig_termios); }

/**
 * @brief Sets the terminal to raw mode.
 *
 * This function configures the terminal to raw mode, where input is not
 * automatically echoed and special characters are not processed. It
 * saves the original terminal settings and registers a function to
 * restore them at exit.
 */
inline void set_terminal_mode() {
    struct termios new_termios;
    tcgetattr(0, &orig_termios);
    memcpy(&new_termios, &orig_termios, sizeof(new_termios));
    atexit(reset_terminal_mode);
    cfmakeraw(&new_termios);
    tcsetattr(0, TCSANOW, &new_termios);
}

/**
 * @brief Checks if a key has been pressed.
 *
 * This function uses the select() system call to check if input is available
 * on the standard input (file descriptor 0) without blocking. It returns
 * a non-zero value if a key has been pressed, and zero otherwise.
 *
 * @return int Non-zero if a key has been pressed, zero otherwise.
 */
inline int kbhit() {
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

/**
 * @brief Gets the pressed key.
 *
 * This function reads a single character from the standard input (file
 * descriptor 0). It returns the character read, or a negative value
 * in case of an error.
 *
 * @return int The character read, or a negative value if an error occurs.
 */
inline int getch() {
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        return c;
    }
}

#endif // !TERMINAL_UTILS_H
