#include "game_manager.hpp"
#include "color.h"
#include "ini_file_parser.h"
#include "maze.hpp"
#include "terminal_utils.h"
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

/// Utils
namespace {
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
bool read_yes_no_confirmation(/* bool yes_preffered */) {
    // FIX: Make a default option, and handle cases where the user inputed anything but "yes" or
    // "no"
    string choice;
    std::cin >> choice;
    std::transform(choice.begin(), choice.end(), choice.begin(), ::tolower);
    return choice == "y";
}
void read_enter_to_proceed() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}
void cin_clear() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
} // namespace

namespace snaze {
SnazeManager::SnazeManager(const std::string &game_levels_directory,
                           const std::string &ini_config_file_path) {
    m_game_levels_files = get_files_from_directory(game_levels_directory);
    auto config_raw = ini::Parser::file(ini_config_file_path);
    m_settings.fps = std::stoi(config_raw.at("").at("game_fps"));
    m_settings.lives = std::stoi(config_raw.at("").at("snake_lives"));
    m_settings.food_amount = std::stoi(config_raw.at("").at("food_amount"));
}

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

Direction SnazeManager::read_starting_direction() {
    set_terminal_mode();
    auto start_direction = getch();
    while (start_direction < 0) {
        start_direction = getch();
    }
    reset_terminal_mode();
    return (Direction)start_direction;
}

void SnazeManager::process() {
    if (m_snaze_state == SnazeState::Init) {
    } else if (m_snaze_state == SnazeState::MainMenu) {
        m_menu_option = read_menu_option();
    } else if (m_snaze_state == SnazeState::Quit) {
        m_asked_to_quit = read_yes_no_confirmation();
    } else if (m_snaze_state == SnazeState::SnazeMode) {
        m_snaze_mode = read_snaze_option();
    } else if (m_snaze_state == SnazeState::GameStart) {
        m_snake.head_direction = read_starting_direction();
    } else {
        read_enter_to_proceed();
    }
    // TODO: process: GameLoop (On, Damage, Game Over, Won)
}

void SnazeManager::change_state_by_selected_menu_option() {
    const std::unordered_map<MainMenuOption, SnazeState> states{
        {MainMenuOption::Play, SnazeState::SnazeMode}, {MainMenuOption::Quit, SnazeState::Quit}};
    auto temp_game_state = states.find(m_menu_option);
    m_snaze_state = (temp_game_state != states.cend()) ? temp_game_state->second : m_snaze_state;
}

void SnazeManager::update() {
    if (not m_system_msg.empty()) {
        return;
    }
    if (m_snaze_state == SnazeState::Init) {
        m_snaze_state = SnazeState::MainMenu;
    } else if (m_snaze_state == SnazeState::MainMenu) {
        change_state_by_selected_menu_option();
        m_menu_option = MainMenuOption::Undefined;
    } else if (m_snaze_state == SnazeState::Quit) {
        m_snaze_state = (m_asked_to_quit) ? m_snaze_state : SnazeState::MainMenu;
    } else if (m_snaze_state == SnazeState::SnazeMode) {
        m_snaze_state = SnazeState::GameStart;
        // NOTE: Picking a random level
        auto random_idx = rand() % m_game_levels_files.size();
        m_maze = Maze(m_game_levels_files[random_idx]);
        m_game_levels_files.erase(m_game_levels_files.cbegin() + (long)random_idx);
    } else {
        m_snaze_state = SnazeState::MainMenu;
    }
    /// TODO: Update: GameLoop (On, Damage, Won, Game Over)
}

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

void SnazeManager::render() {
    clear_screen();
    if (m_snaze_state == SnazeState::MainMenu) {
        screen_title("Snaze Game 🐍");
        main_content(main_menu_mc());
        interaction_msg("Select one option and press enter");
    } else if (m_snaze_state == SnazeState::Quit) {
        screen_title("Quitting");
        main_content("Do you want to quit the snaze game? ");
        interaction_msg("[Y/n]");
    } else if (m_snaze_state == SnazeState::SnazeMode) {
        screen_title("Snaze Mode");
        main_content(snaze_mode_mc());
        interaction_msg("Select one option and press enter");
    } else if (m_snaze_state == SnazeState::GameStart) {
        main_content(m_maze.str()); // HACK: Change to correct maze string method
    } else {
        screen_title("WORK IN PROGRESS 🛠️");
        main_content("Sorry that function isn't implemented yet 😓\n\n");
        interaction_msg("Press <Enter> to go back");
    }
    // TODO: Render: GameLoop (On, Damage, Game Over, Won)
    if (not m_screen_title.empty()) {
        std::cout << screen_title();
        m_screen_title.clear();
    }
    if (not m_main_content.empty()) {
        std::cout << main_content();
        m_main_content.clear();
    }
    if (not m_system_msg.empty()) {
        std::cout << system_msg();
        m_system_msg.clear();
    }
    if (not m_interaction_msg.empty()) {
        std::cout << interaction_msg() << std::flush;
        m_interaction_msg.clear();
    }
}
} // namespace snaze
