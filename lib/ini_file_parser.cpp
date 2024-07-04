#include "include/ini_file_parser.h"
#include "../src/include/game_manager.hpp"
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/types.h>

namespace ini {

std::string Parser::trim(std::string line) {
    line.erase(0, line.find_first_not_of(" \t"));
    line.erase(line.find_last_not_of(" \t") + 1);
    return line;
}

std::ifstream Parser::open_file(const std::string &file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open file " + file_path);
    }
    return file;
}

snaze::Settings convert_map_to_settings(const Parser::IniUMap &map) {
    snaze::Settings settings;

    for (const auto &[sec, key_val] : map) {
        for (const auto &[key, val] : key_val) {
            if (key == "food_amount") {
                settings.food_amount = std::stoi(val);
            } else if (key == "snake_lives") {
                settings.lives = std::stoi(val);
            } else if (key == "game_fps") {
                settings.fps = std::stoi(val);
            } else if (key == "player_type") {
                settings.player_type = val;
            } else {
                throw std::invalid_argument("Unpredicted value");
            }
        }
    }

    return settings;
}

snaze::Settings Parser::file(const std::string &settings_path) {
    Parser::IniUMap map;
    auto file = open_file(settings_path);
    std::string line;
    std::string current_section;
    while (getline(file, line)) {
        line = trim(line);
        // Comment or empty
        if (line.empty() or line.front() == ';' or line.front() == '#') {
            continue;
        }
        // Section [section]
        if (line.front() == '[' and line.back() == ']') {
            current_section = line.substr(1, line.size() - 2);
            continue;
        }
        // var = val
        auto equal_pos = line.find('=');
        if (equal_pos != std::string::npos) {
            auto var = trim(line.substr(0, equal_pos));
            auto value = trim(line.substr(equal_pos + 1));
            map[current_section][var] = value;
        }
    }

    return convert_map_to_settings(map);
}

} // namespace ini
