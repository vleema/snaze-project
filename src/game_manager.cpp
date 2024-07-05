#include "game_manager.hpp"
#include "maze.hpp"
#include "snake.hpp"
#include "terminal_utils.h"
#include "utils.hpp"

#include <cstddef>
#include <cstdlib>
#include <experimental/random>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace snaze {

void SnazeManager::process() {
    if (m_snaze_state == SnazeState::Init) {
    } else if (m_snaze_state == SnazeState::MainMenu) {
        m_menu_option = read_menu_option();
    } else if (m_snaze_state == SnazeState::Quit) {
        m_asked_to_quit = read_yes_no_confirmation(false);
    } else if (m_snaze_state == SnazeState::SnazeMode) {
        m_snaze_mode = read_snaze_option();
    } else if (m_snaze_state == SnazeState::BotMode) {
        m_bot_strategy = read_bot_option();
    } else if (m_snaze_state == SnazeState::GameStart) {
        m_snake.reset();
        m_maze.random_food_position();

        if (m_snaze_mode == SnazeMode::Bot) {
            m_snake.body.push_front(m_maze.start());
            snake_bot_think(m_snake);
            // std::cerr << '\n' << m_maze.str_debug(m_snake_bot.solution.value());
            // exit(0);
            return;
        }
        m_snake.head_direction = read_starting_direction();
        m_snake.body.push_back(m_maze.start() + m_snake.head_direction);
    } else if (m_snaze_state == SnazeState::On) {
        if (m_snaze_mode == SnazeMode::Player) {
            set_terminal_mode();
            if (kbhit() != 0) {
                m_snake.head_direction = input(static_cast<char>(getch()), m_snake.head_direction);
            }
            reset_terminal_mode();
        } else if (m_snaze_mode == SnazeMode::Bot) {
            if (m_snake_bot.solution.has_value() and m_snake_bot.solution.value().empty()) {
                snake_bot_think(m_snake);
            }
            m_snake.head_direction = m_snake_bot.solution.value().front();
            m_snake_bot.solution.value().pop_front();
        }
    } else if (m_snaze_state == SnazeState::Won or m_snaze_state == SnazeState::Lost) {
        m_snaze_state =
            (read_yes_no_confirmation(true)) ? SnazeState::SnazeMode : SnazeState::MainMenu;
    } else if (m_snaze_state == SnazeState::Damage) {
        if (m_snaze_mode != SnazeMode::Bot) {
            return;
        }
        read_enter_to_proceed();
        /// HACK: Maybe go directly to GameStart, is quite annoying pressing enter
    } else {
        read_enter_to_proceed();
    }
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
        if (m_snaze_mode == SnazeMode::Bot) {
            m_snaze_state = SnazeState::BotMode; // Redirect to the bot selection screen
        } else {
            m_snaze_state = SnazeState::GameStart; // Playing manual
        }
        // NOTE: Picking a random level
        if (still_levels_available()) {
            size_t random_idx = std::experimental::randint(0, (int)m_game_levels_files.size() - 1);
            m_maze = Maze(m_game_levels_files[random_idx]);
            m_game_levels_files.erase(m_game_levels_files.cbegin() + (long)random_idx);
        } else if (m_remaining_snake_lives > 0) {
            m_snaze_state = SnazeState::Won;
        }
        m_score = 0;
        m_eaten_food_amount_snake = 0;
        m_remaining_snake_lives = m_settings.lives;
    } else if (m_snaze_state == SnazeState::BotMode) {
        if (m_bot_strategy !=
            BotMode::Undefined) { // Just let the user leave if a valid opt was picked
            m_snaze_state = SnazeState::GameStart;
        }
    } else if (m_snaze_state == SnazeState::GameStart) {
        m_snaze_state = SnazeState::On;
    } else if (m_snaze_state == SnazeState::On) {
        bool ate = false;
        auto updated_snake_head_position = update_snake_position();
        if (m_maze.blocked(updated_snake_head_position, Direction::None) or
            m_snake.is_snake_body(updated_snake_head_position)) {
            m_snaze_state = SnazeState::Damage;
        } else if (m_maze.found_food(updated_snake_head_position)) {
            if (++m_eaten_food_amount_snake == m_settings.food_amount) {
                m_snaze_state = SnazeState::Won;
            }
            m_maze.random_food_position();
            ate = true;
        }
        if (not ate) {
            m_snake.body.pop_back();
        }
    } else if (m_snaze_state == SnazeState::Won or m_snaze_state == SnazeState::Lost) {
        m_new_game = true;
    } else if (m_snaze_state == SnazeState::Damage) {
        if (--m_remaining_snake_lives == 0) {
            m_snaze_state = SnazeState::Lost;
            return;
        }
        m_snaze_state = SnazeState::GameStart;
        m_snake.reset();
        // HACK: Maybe reset food also cause still showing on screen
    } else {
        m_snaze_state = SnazeState::MainMenu;
    }
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
        interaction_msg("[y/N]");
    } else if (m_snaze_state == SnazeState::SnazeMode) {
        screen_title("Snaze Mode");
        main_content(snaze_mode_mc());
        interaction_msg("Select one option and press enter");
    } else if (m_snaze_state == SnazeState::BotMode) {
        screen_title("Select bot type:");
        main_content(bot_mode_mc());
        interaction_msg("Select one option and press enter");
    } else if (m_snaze_state == SnazeState::GameStart) {
        main_content(m_maze.str_spawn());
        interaction_msg(controls_im());
    } else if (m_snaze_state == SnazeState::On) {
        main_content(game_loop_mc());
    } else if (m_snaze_state == SnazeState::Damage) {
        main_content(game_loop_info() + '\n' + m_maze.str_spawn());
        interaction_msg("The bot has suffered damage!!! Press <Enter> to continue");
    } else if (m_snaze_state == SnazeState::Won) {
        screen_title("The Snake has found it's way!");
        main_content("This snake badass as fuck fr\n\n");
        interaction_msg("Do you want to continue with the snaze? [Y/n]");
    } else if (m_snaze_state == SnazeState::Lost) {
        screen_title("The snake got wrecked");
        main_content("Unfortunaley the snaked as passed 🙁\n");
        interaction_msg("Do you want to continue with the snaze? [Y/n]");
    } else {
        screen_title("WORK IN PROGRESS 🛠️");
        main_content("Sorry that function isn't implemented yet 😓\n\n");
        interaction_msg("Press <Enter> to go back");
    }
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
    if (m_snaze_state == SnazeState::On) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / m_settings.fps));
    }
}
} // namespace snaze
