#include "snake.hpp"
#include "maze.hpp"

#include <deque>
#include <experimental/random>
#include <optional>
#include <utility>
namespace snaze {
SnakeBot::MaybeDirectionDeque SnakeBot::solve(const Maze &maze, const Snake &snake) {
    // TODO: Account for snake body
    std::queue<std::pair<Position, Snake>> to_visit;
    std::unordered_set<Position, Position::Hash> meta_visited;
    std::map<Position, Direction> came_from;
    constexpr std::array directions{Direction::Up, Direction::Down, Direction::Left,
                                    Direction::Right};
    auto start_pos = snake.body.front();
    to_visit.push({start_pos, snake});
    meta_visited.insert(start_pos);
    while (not to_visit.empty()) {
        auto [current_pos, current_snake] = to_visit.front();
        to_visit.pop();
        if (maze.found_food(current_pos)) {
            return reconstruct_path(came_from, start_pos, current_pos);
        }
        for (const auto &dir : directions) {
            Snake next_snake = current_snake;
            next_snake.move_snake(dir);
            auto next_pos = next_snake.body.front();
            if (next_snake.is_snake_body(next_pos)) {
                continue;
            }
            if (maze.in_bound(next_pos) and not(maze.blocked(current_pos, dir)) and
                not already_visited(next_pos, meta_visited)) {
                came_from[next_pos] = dir;
                to_visit.push({next_pos, next_snake});
                meta_visited.insert(next_pos);
            }
        }
    }
    return std::nullopt;
}

std::vector<Direction> SnakeBot::positions_available(const Maze &maze, const Snake &snake) {
    std::vector<Direction> moves;
    for (const auto &dir : {Direction::Up, Direction::Down, Direction::Right, Direction::Left}) {
        if (maze.blocked(snake.body.front(), dir) &&
            snake.is_snake_body(snake.body.front() + dir)) {
            continue;
        }
        moves.push_back(dir);
    }
    return moves;
}

SnakeBot::MaybeDirectionDeque SnakeBot::play_random(const Maze &maze, const Snake &snake) {
    MaybeDirectionDeque moves;
    auto available_moves = positions_available(maze, snake);
    if (available_moves.empty()) {
        return std::deque({snake.head_direction});
    }
    auto head_dir = available_moves[std::experimental::randint(0, (int)available_moves.size() - 1)];

    return std::deque({head_dir});
}
} // namespace snaze
