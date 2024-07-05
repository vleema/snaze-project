#include "color.h"
#include "game_manager.hpp"
#include "ini_file_parser.h"
#include "maze.hpp"
#include "snake.hpp"
#include "terminal_utils.h"
#include "utils.hpp"

#include <cstddef>
#include <cstdlib>
#include <experimental/random>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using namespace snaze;
SnazeManager::MainMenuOption SnazeManager::read_menu_option() {
    int choice = 0;
    std::cin >> choice;
    if (std::cin.fail() or choice < (int)MainMenuOption::Play or
        choice > (int)MainMenuOption::Quit) {
        cin_clear();
        system_msg("Invalid option, try again");
        return MainMenuOption::Undefined;
    }
    return (MainMenuOption)choice;
}

SnazeManager::SnazeMode SnazeManager::read_snaze_option() {
    int choice = 0;
    std::cin >> choice;
    if (std::cin.fail() or choice < (int)SnazeMode::Player or choice > (int)SnazeMode::Bot) {
        cin_clear();
        system_msg("Invalid option, try again");
        return SnazeMode::Undefined;
    }
    return (SnazeMode)choice;
}

SnazeManager::BotMode SnazeManager::read_bot_option() {
    int choice = 0;
    std::cin >> choice;
    if (std::cin.fail() or choice < (int)BotMode::Smart or choice > (int)BotMode::Dumb) {
        cin_clear();
        system_msg("Invalid option, try again");
        return BotMode::Undefined;
    }
    return (BotMode)choice;
}

Direction SnazeManager::read_starting_direction() {
    set_terminal_mode();
    auto start_direction = getch();
    while (start_direction < 0) {
        start_direction = getch();
    }
    reset_terminal_mode();
    return (Direction)start_direction;
}

Direction SnazeManager::input(char keystroke, Direction previous_direction) {
    Direction input_result = Direction::None;
    switch (keystroke) {
    case 'w':
        input_result = (previous_direction != Direction::Down) ? Direction::Up : previous_direction;
        break;
    case 's':
        input_result = (previous_direction != Direction::Up) ? Direction::Down : previous_direction;
        break;
    case 'a':
        input_result =
            (previous_direction != Direction::Right) ? Direction::Left : previous_direction;
        break;
    case 'd':
        input_result =
            (previous_direction != Direction::Left) ? Direction::Right : previous_direction;
        break;
    }
    return input_result;
}

void SnazeManager::snake_bot_think(const Snake &snake) {
    m_snake_bot.solution = SnakeBot::solve(m_maze, snake);
    if (not m_snake_bot.solution.has_value()) {
        m_snake_bot.solution = SnakeBot::play_random(m_maze, snake);
        if (!m_snake_bot.solution.has_value()) {
            throw std::runtime_error("Something went wrong, while the bot was thinking");
        }
    }
}

//
// RENDERING
//
void SnazeManager::screen_title(std::string new_screen_title) {
    m_screen_title = std::move(new_screen_title);
}

void SnazeManager::main_content(std::string new_main_content) {
    m_main_content = std::move(new_main_content);
}

void SnazeManager::system_msg(std::string new_system_msg) {
    m_system_msg = std::move(new_system_msg);
}

void SnazeManager::interaction_msg(std::string new_interaction_msg) {
    m_interaction_msg = std::move(new_interaction_msg);
}

std::string SnazeManager::screen_title() const {
    constexpr int PADDING_SIZE = 10;
    ostringstream screen_title_oss;
    ostringstream padding1_oss;
    ostringstream padding2_oss;
    padding1_oss << '=' << std::setw(PADDING_SIZE) << std::setfill('-') << "[ ";
    padding2_oss << " ]" << std::setw(PADDING_SIZE) << std::setfill('-') << '=';
    screen_title_oss << padding1_oss.str() << m_screen_title << padding2_oss.str() << "\n\n";
    return Color::tcolor(screen_title_oss.str(), Color::BOLD);
}

std::string SnazeManager::main_content() const { return m_main_content; }

std::string SnazeManager::system_msg() const {
    std::ostringstream oss;
    oss << "[ERROR: " << m_system_msg << "]\n";
    return oss.str();
}

std::string SnazeManager::interaction_msg() const {
    std::ostringstream oss;
    oss << m_interaction_msg << " > ";
    return oss.str();
}

std::string SnazeManager::main_menu_mc() {
    std::ostringstream oss;
    oss << "[1] - Play\n"
        << "[2] - Quit\n\n";
    return oss.str();
}

std::string SnazeManager::snaze_mode_mc() {
    std::ostringstream oss;
    oss << "[1] - Normal\n"
        << "[2] - Bot\n\n";
    return oss.str();
}

std::string SnazeManager::bot_mode_mc() {
    std::ostringstream oss;
    oss << "[1] - Smart bot\n"
        << "[2] - Dumb bot\n";
    return oss.str();
}

std::string line(size_t n) {
  std::ostringstream oss;

  for(size_t i = 0; i < n; ++i) {
      oss << "=";
  }
  return oss.str();
}

std::string SnazeManager::game_loop_info() const {
    // ♥︎ ☠
    constexpr char heart[] = "♥︎";
    constexpr char skull[] = "☠";
    std::ostringstream header_oss;
    std::ostringstream padding_oss;
    size_t line_length = 50;
    header_oss << line(line_length) << std::endl << "Lives: ";
    for (size_t i = 0; i < m_remaining_snake_lives; ++i) {
        header_oss << " " << heart;
    }
    for (size_t i = 0; i < (size_t)(m_settings.lives - m_remaining_snake_lives); ++i) {
        header_oss << " " << skull;
    }
    header_oss << " | Score: 0 | Food eaten " << m_eaten_food_amount_snake << " of "
               << m_settings.food_amount << std::endl << line(line_length) << std::endl;
    auto header_str = header_oss.str();
    padding_oss << std::setw((int)header_str.size()) << std::setfill('-');
    return header_str;
}

std::string SnazeManager::game_loop_mc() const {
    std::ostringstream oss;
    oss << game_loop_info() << '\n';
    oss << m_maze.str_in_game(m_snake.body, m_snake.head_direction);
    // TODO: Add multiples '-' before the maze

    return oss.str();
}

std::string SnazeManager::controls_im() const {
    std::ostringstream oss;
    oss << "Press the following keys to play: " << std::endl
        << "W - UP" << std::endl
        << "S - DOWN " << std::endl
        << "D - RIGHT " << std::endl
        << "A - LEFT" << std::endl;

    return oss.str();
}

Position SnazeManager::update_snake_position() {
    switch (m_snake.head_direction) {
    case Direction::Up:
        m_snake.body.emplace_front(m_snake.body[0].coord_x,
                                   (m_snake.body[0].coord_y - 1 % m_maze.height()) %
                                       m_maze.height());
        break;
    case Direction::Down:
        m_snake.body.emplace_front(m_snake.body[0].coord_x,
                                   (m_snake.body[0].coord_y + 1 % m_maze.height()) %
                                       m_maze.height());
        break;
    case Direction::Left:
        m_snake.body.emplace_front((m_snake.body[0].coord_x - 1 % m_maze.width()) % m_maze.width(),
                                   m_snake.body[0].coord_y);
        break;
    case Direction::Right:
        m_snake.body.emplace_front((m_snake.body[0].coord_x + 1 % m_maze.width()) % m_maze.width(),
                                   m_snake.body[0].coord_y);
        break;
    }
    return m_snake.body.front();
}

std::vector<std::string> get_files_from_directory(const std::string &dir_name) {
    namespace fs = std::filesystem;
    fs::path dir_path = dir_name;
    std::vector<std::string> file_list;
    if (not fs::is_directory(dir_path)) {
        throw std::invalid_argument(dir_name + " Is not a directory");
    }
    for (const auto &entry : fs::directory_iterator(dir_name)) {
        if (fs::is_regular_file(entry)) {
            file_list.emplace_back(entry.path().string());
        }
    }
    return file_list;
}

bool SnazeManager::still_levels_available() { return m_game_levels_files.size() != 0; }

SnazeManager::SnazeManager(const std::string &game_levels_directory,
                           const std::string &ini_config_file_path) {
    m_game_levels_files = get_files_from_directory(game_levels_directory);
    m_settings = ini::Parser::file(ini_config_file_path);
}

void SnazeManager::change_state_by_selected_menu_option() {
    const std::unordered_map<MainMenuOption, SnazeState> states{
        {MainMenuOption::Play, SnazeState::SnazeMode}, {MainMenuOption::Quit, SnazeState::Quit}};
    auto temp_game_state = states.find(m_menu_option);
    m_snaze_state = (temp_game_state != states.cend()) ? temp_game_state->second : m_snaze_state;
}

// read opt
// confirmations
// update utils
// render utils
