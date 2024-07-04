#include <algorithm>
#include <chrono> // std::chrono::milliseconds
#include <cmath>
#include <cstring> // memcpy
#include <deque>
#include <experimental/random>
#include <iostream>  // std::cout
#include <termios.h> // struct termios
#include <thread>    // std::this_thread::sleep_for
#include <unistd.h>  // system("clear")

// Define the initial position of the player
constexpr int WIDTH = 40, HEIGHT = 20;

int foodX, foodY;

std::deque<std::pair<int, int>> snake = {
    {std::experimental::randint(0, WIDTH - 1), std::experimental::randint(0, HEIGHT - 1)}};

enum Direction { UP = 'w', DOWN = 's', LEFT = 'a', RIGHT = 'd' };
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
            auto has_snake_body = [&](const std::pair<int, int> &p) {
                return p.first == x && p.second == y;
            };
            if (std::any_of(snake.begin(), snake.end(), has_snake_body)) {
                std::cout << 'S'; // Draw the player
            } else if (x == foodX && y == foodY) {
                std::cout << 'F';
            } else {
                std::cout << '.';
            }
        }
        std::cout << '\n';
        std::cout.flush();
    }
}

void generate_food() {
    foodX = std::experimental::randint(0, WIDTH - 1);
    foodY = std::experimental::randint(0, HEIGHT - 1);

    while (foodX == snake[0].first && foodY == snake[0].second) {
        foodX = std::experimental::randint(0, WIDTH - 1);
        foodY = std::experimental::randint(0, HEIGHT - 1);
    }
}

// Function to handle player input and update position
void input() {
    if (kbhit()) { // Check if a key has been pressed
        switch (getch()) {
        // TODO: add body avoid as a condition
        case 'w':
            player_direction = (player_direction != DOWN) ? UP : player_direction;
            break;
        case 's':
            player_direction = (player_direction != UP) ? DOWN : player_direction;
            break;
        case 'a':
            player_direction = (player_direction != RIGHT) ? LEFT : player_direction;
            break;
        case 'd':
            player_direction = (player_direction != LEFT) ? RIGHT : player_direction;
            break;
        case 'q': // Press 'q' to quit the game
            exit(0);
        }
    }
}

// Function to update
void update() {
    bool ate = false;

    if (snake[0].first == foodX && snake[0].second == foodY) {
        generate_food();
        ate = true;
    }

    // TODO: add body avoid as a condition
    switch (player_direction) {
    case UP:
        snake.emplace_front(snake[0].first, (snake[0].second - 1 % HEIGHT) % HEIGHT);
        break;
    case DOWN:
        snake.emplace_front(snake[0].first, (snake[0].second + 1 % HEIGHT) % HEIGHT);
        break;
    case LEFT:
        snake.emplace_front((snake[0].first - 1 % WIDTH) % WIDTH, snake[0].second);
        break;
    case RIGHT:
        snake.emplace_front((snake[0].first + 1 % WIDTH) % WIDTH, snake[0].second);
        break;
    }
    if (!ate) {
        snake.pop_back();
    }
}

int main() {
    generate_food();
    draw();
    set_terminal_mode();
    auto initial_input = getch();
    while (initial_input < 0) {
        initial_input = getch();
    }
    player_direction = (Direction)initial_input;
    reset_terminal_mode();
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
