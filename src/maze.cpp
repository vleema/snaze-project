#include "maze.hpp"

#include <fstream>
#include <istream>
#include <list>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "color.h"

namespace {
std::optional<std::ifstream> open_file(const std::string &filename) {
    std::ifstream ifs_file(filename);
    if (not ifs_file.is_open()) {
        return std::nullopt;
    }
    return ifs_file;
}

std::optional<std::pair<size_t, size_t>> read_array_dimensions(const std::string &line) {
    std::istringstream iss(line);
    size_t height = 0;
    size_t width = 0;
    iss >> height >> width;
    if (iss.fail() or iss.bad()) {
        return std::nullopt;
    }
    return std::make_pair(height, width);
}
} // namespace

namespace snaze {
Maze::Maze(const std::string &filename) : m_spawn(0, 0), m_food(0, 0) {
    auto file = open_file(filename);
    if (not file.has_value()) {
        throw std::invalid_argument("Couldn't open file: " + filename);
    }
    std::string file_line;
    size_t line_count = 0;
    bool first_line = true;
    while (std::getline(file.value(), file_line)) {
        if (first_line) {
            auto dimensions = read_array_dimensions(file_line);
            if (not dimensions.has_value()) {
                throw std::invalid_argument("Failed in reading header for maze file");
            }
            m_height = dimensions.value().first;
            m_width = dimensions.value().second;
            resize_maze();
            first_line = false;
            continue;
        }
        size_t col_count = 0;
        for (const auto &chr : file_line) {
            auto cell = (Cell)chr;
            if (cell == Cell::Spawn) {
                m_spawn = Position(col_count, line_count);
            } else if (cell == Cell::Free) {
                m_free_cells.emplace_back(line_count, col_count);
            }
            m_maze[line_count][col_count++] = cell;
        }
        line_count++;
    }
    // FIX: Error treatment for problematic levels
    // TODO: Functionality to read a file with multiple levels
}

std::string Maze::str() const {
    constexpr char wall[] = "█";
    constexpr char free = ' ';
    constexpr char spawn[] = "꩜";
    constexpr char food[] = "🥚";
    constexpr char path[] = "█";
    std::ostringstream oss;
    for (const auto &row : m_maze) {
        for (const auto &cell : row) {
            if (cell == Cell::Free or cell == Cell::InvisibleWall) {
                oss << free;
            } else if (cell == Cell::Wall) {
                oss << Color::tcolor(wall, Color::GREEN);
            } else if (cell == Cell::Spawn) {
                oss << Color::tcolor(spawn, Color::YELLOW);
            } else if (cell == Cell::Food) {
                oss << Color::tcolor(food, Color::MAGENTA);
            } else if (cell == Cell::Path) {
                oss << Color::tcolor(path, Color::RED);
            }
        }
        oss << '\n';
    }
    oss << '\n'
        << Color::tcolor(spawn, Color::YELLOW) << " - Spawn\n"
        << Color::tcolor(food, Color::MAGENTA) << " - Food\n";
    return oss.str();
}

std::string Maze::str(std::list<Direction> &solution) const {
    auto maze_copy(*this);
    auto current_pos = start();

    for (const auto &dir : solution) {
        current_pos = current_pos + dir;
        maze_copy.m_maze[current_pos.coord_y][current_pos.coord_x] =
            (current_pos != m_food) ? Cell::Path : Cell::Food;
    }
    return maze_copy.str();
}

void Maze::random_food_position() {
    m_food = m_free_cells[rand() % m_free_cells.size()];
    m_maze[m_food.coord_y][m_food.coord_x] = Cell::Food;
}
} // namespace snaze
