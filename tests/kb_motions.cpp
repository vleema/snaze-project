#include <chrono>    // std::chrono::milliseconds
#include <cstring>   // memcpy
#include <iostream>  // std::cout
#include <termios.h> // struct termios
#include <thread>    // std::this_thread::sleep_for
#include <unistd.h>  // system("clear")

// Define the initial position of the player
int playerX = 10;
int playerY = 10;

constexpr int WIDTH = 20, HEIGHT = 20;

enum Direction { UP, DOWN, LEFT, RIGHT };
Direction player_direction = RIGHT;

struct termios orig_termios;

// Function to reset terminal to original state
void reset_terminal_mode() { tcsetattr(0, TCSANOW, &orig_termios); }

// Function to set terminal to raw mode
void set_terminal_mode() {
    struct termios new_termios;
    tcgetattr(0, &orig_termios);
    memcpy(&new_termios, &orig_termios, sizeof(new_termios));
    atexit(reset_terminal_mode);
    cfmakeraw(&new_termios);
    tcsetattr(0, TCSANOW, &new_termios);
}

// Function to check if a key has been pressed
int kbhit() {
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

// Function to get the pressed key
int getch() {
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        return c;
    }
}

// Function to draw the game state
void draw() {
    system("clear"); // Clear the screen
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            if (x == playerX && y == playerY)
                std::cout << 'P'; // Draw the player
            else
                std::cout << '.';
        }
        std::cout << '\n';
        std::cout.flush();
    }
}

// Function to handle player input and update position
void input() {
    if (kbhit()) { // Check if a key has been pressed
        switch (getch()) {
        case 'w':
            player_direction =
                (player_direction != DOWN) ? UP : player_direction;
            break;
        case 's':
            player_direction =
                (player_direction != UP) ? DOWN : player_direction;
            break;
        case 'a':
            player_direction =
                (player_direction != RIGHT) ? LEFT : player_direction;
            break;
        case 'd':
            player_direction =
                (player_direction != LEFT) ? RIGHT : player_direction;
            break;
        case 'q': // Press 'q' to quit the game
            exit(0);
        }
    }
}

// Function to update
void update() {
    switch (player_direction) {
    case UP:
        playerY = (playerY > 0) ? playerY - 1 : playerY;
        break;
    case DOWN:
        playerY = (playerY < WIDTH - 1) ? playerY + 1 : playerY;
        break;
    case LEFT:
        playerX = (playerX > 0) ? playerX - 1 : playerX;
        break;
    case RIGHT:
        playerX = (playerX < HEIGHT - 1) ? playerX + 1 : playerX;
        break;
    }
}
int main() {
    while (true) {
        set_terminal_mode();
        input();
        reset_terminal_mode();
        update();
        draw();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}
