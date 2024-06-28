#ifndef TERMINAL_UTILS_H
#define TERMINAL_UTILS_H

#include <cstdlib>
#include <string>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

/**
 * @brief Resets the terminal to its original state.
 *
 * This function restores the terminal settings to the state they were in
 * before the program was started, typically used when exiting the program.
 */
void reset_terminal_mode();

/**
 * @brief Sets the terminal to raw mode.
 *
 * This function configures the terminal to raw mode, where input is not
 * automatically echoed and special characters are not processed. It
 * saves the original terminal settings and registers a function to
 * restore them at exit.
 */
void set_terminal_mode();

/**
 * @brief Checks if a key has been pressed.
 *
 * This function uses the select() system call to check if input is available
 * on the standard input (file descriptor 0) without blocking. It returns
 * a non-zero value if a key has been pressed, and zero otherwise.
 *
 * @return int Non-zero if a key has been pressed, zero otherwise.
 */
int kbhit();

/**
 * @brief Gets the pressed key.
 *
 * This function reads a single character from the standard input (file
 * descriptor 0). It returns the character read, or a negative value
 * in case of an error.
 *
 * @return int The character read, or a negative value if an error occurs.
 */
int getch();

/**
 * @brief Retrieves the current width of the terminal in characters.
 *
 * This function uses the `ioctl` system call to get the terminal's
 * window size and returns the number of columns (characters) in the
 * terminal window.
 *
 * @return The width of the terminal in characters.
 */
int get_terminal_width();

/**
 * @brief Prints a string centered in the terminal.
 *
 * This function calculates the necessary padding to center the provided
 * text in the terminal window and then prints it.
 *
 * @param text The string to be printed centered in the terminal.
 */
void print_centered(const std::string &text);

#endif // !TERMINAL_UTILS_H
